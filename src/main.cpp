#include <aws/core/Aws.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/lambda-runtime/runtime.h>
#include <string>
#include <thread>
#include <chrono>
#include "config.h"
#include "hash.h"
#include "salt.h"
#include "utils/json.h"
#include "db.h"

#ifndef PI_SECRET
    #define PI_SECRET ""
#endif

using namespace aws::lambda_runtime;

invocation_response handler(invocation_request const &request)
{
  JsonValue json(Aws::String(request.payload));
  if (!json.WasParseSuccessful()) return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
  auto payload = json.View();

  if (!isType(payload, "method", String))
    return invocation_response::failure("Missing method", "InvalidJSON");

  std::string response = "";
  std::string error = "";
  auto method = payload.GetString("method");

  if (method == "hash" || method == "set") {
    if (strcmp(PI_SECRET, "") == 0) error = "missing secret";
    else if (!isType(payload, "password", String)) error = "missing password";
    else if (method == "set" && !isType(payload, "user", String)) error = "missing user";
  }
  uint8_t salt[SALTLEN];
  if (!saltFromUUIDv4(request.request_id.c_str(), salt)) error = "couldn't generate salt";
  if (error != "") return invocation_response::failure(error, error);

  if (method == "hash") {
    try {
      Argon2Params params;
      if (isType(payload, "time", Integer)) params.time_cost = payload.GetInteger("time");
      if (isType(payload, "memory", Integer)) params.memory_cost = payload.GetInteger("memory");
      if (isType(payload, "parallelism", Integer)) params.parallelism = payload.GetInteger("parallelism");
      params.secret = PI_SECRET;

      auto t0 = std::chrono::high_resolution_clock::now();
      auto hashed = hash(payload.GetString("password"), salt, params);
      auto t1 = std::chrono::high_resolution_clock::now();
      auto td = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0);

      response = "{\"hashed\":\"" + hashed + 
                 "\",\"params\":" + params.toJSON() +
                 ",\"durUS\":" + std::to_string(td.count()) + "}";
    } catch (const char* err) {
      response = std::string(err ?: "internal error");
      error = "EncodingFailed";
    }
  }
  else if (method == "set") {
    auto hashed = hash(payload.GetString("password"), salt);
    DBClient db;
    db.putItem(payload.GetString("user").c_str(), hashed.c_str());
  }
  else if (method == "info") {
    auto threads = std::thread::hardware_concurrency();
    response = "{\"threads\":" + std::to_string(threads) + "}";
  }
  else {
    error = "invalid method";
  }

  return error == ""
    ? invocation_response::success(response, "application/json")
    : invocation_response::failure(response, error);
}

int main()
{
  run_handler(handler);
  return 0;
}
