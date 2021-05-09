#include <aws/core/Aws.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/lambda-runtime/runtime.h>
#include <string>
#include "config.h"
#include "hash.h"
#include "salt.h"
#include "utils/json.h"

using namespace aws::lambda_runtime;

invocation_response handler(invocation_request const &request)
{
  JsonValue json(request.payload);
  if (!json.WasParseSuccessful()) return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
  auto payload = json.View();

  if (!isType(payload, "method", String))
    return invocation_response::failure("Missing method", "InvalidJSON");

  std::string response = "";
  std::string error = "";

  auto method = payload.GetString("method");
  if (method == "hash") {
    if (!isType(payload, "password", String)) {
      error = "missing password";
    } else {
      auto password = payload.GetString("password");
      try {
        uint8_t salt[SALTLEN];
        if (!saltFromUUIDv4(request.request_id.c_str(), salt)) throw "couldn't generate salt";
        Argon2Params params;
        if (isType(payload, "time", Integer)) params.time_cost = payload.GetInteger("time");
        if (isType(payload, "memory", Integer)) params.memory_cost = payload.GetInteger("memory");
        if (isType(payload, "parallelism", Integer)) params.parallelism = payload.GetInteger("parallelism");
        auto hashed = hash(password.c_str(), salt, params);
        response = "{\"hashed\":\"" + hashed + "\",\"params\":" + params.toJSON() + "}";
      } catch (const char* err) {
        response = err ?: "internal error";
        error = "EncodingFailed";
      }
    }
  }
  else if (method == "params") {
    Argon2Params params;
    response = "{\"params\":" + params.toJSON() + "\"";
  }
  else error = "invalid method";

  return error == ""
    ? invocation_response::success(response, "application/json")
    : invocation_response::failure(response, error);
}

int main()
{
  run_handler(handler);
  return 0;
}
