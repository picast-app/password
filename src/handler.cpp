#include "handler.h"

method_code str2Method(std::string method) {
  if (method == "hash") return HASH;
  if (method == "set") return SET;
  if (method == "info") return INFO;
  return UNKNOWN;
}

invocation_result handleRequest(std::string method, Aws::Utils::Json::JsonView& payload, const uint8_t* const salt, std::string secret) {
  std::string hashed;
  std::chrono::microseconds hash_time;
  Argon2Params params;
  params.secret = secret.c_str();
  if (isType(payload, "time", Integer)) params.time_cost = payload.GetInteger("time");
  if (isType(payload, "memory", Integer)) params.memory_cost = payload.GetInteger("memory");
  if (isType(payload, "parallelism", Integer)) params.parallelism = payload.GetInteger("parallelism");

  switch (str2Method(method)) {
    case INFO:
      return invocation_result(true, "{\"threads\":" + std::to_string(std::thread::hardware_concurrency()) + "}");
    case HASH:
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        hashed = hash(payload.GetString("password"), salt, params);
        auto t1 = std::chrono::high_resolution_clock::now();
        hash_time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        return invocation_result(true, "{\"hashed\":\"" + hashed + 
                                       "\",\"params\":" + params.toJSON() +
                                       ",\"durUS\":" + std::to_string(hash_time.count()) + "}");
      }
    case SET:
      DBClient().putItem(payload.GetString("user"), hash(payload.GetString("password"), salt, params));
      return invocation_result(true, "");
    default:
      return invocation_result(false, "unknown method");
  }
}
