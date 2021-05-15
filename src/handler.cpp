#include "handler.h"

method_code str2Method(std::string method) {
  if (method == "hash") return HASH;
  if (method == "check") return CHECK;
  if (method == "set") return SET;
  if (method == "info") return INFO;
  if (method == "exists") return EXISTS;
  if (method == "delete") return DELETE;
  return UNKNOWN;
}

const std::map<method_code, std::vector<std::pair<std::string, JsonType>>> required = {
  { HASH,   {{ "password", String }} },
  { HASH,   {{ "password", String }} },
  { SET,    {{ "password", String }, { "user", String }, { "id", String }} },
  { CHECK,  {{ "password", String }, { "user", String }} },
  { EXISTS, {{ "user", String }} },
  { DELETE, {{ "user", String }} }
};

invocation_result handleRequest(std::string method_name, Aws::Utils::Json::JsonView& payload, const uint8_t* const salt, std::string secret) {
  auto method = str2Method(method_name);

  if (required.find(method) != required.end()) {
    for (auto [key, type] : required.at(method)) {
      if (!json::isType(payload, key, type))
        return invocation_result(false, "missing field " + key);
    }
  }
  
  std::string hashed;
  std::chrono::microseconds hash_time;
  Argon2Params params;
  params.secret = secret.c_str();
  if (json::isType(payload, "time", Integer)) params.time_cost = payload.GetInteger("time");
  if (json::isType(payload, "memory", Integer)) params.memory_cost = payload.GetInteger("memory");
  if (json::isType(payload, "parallelism", Integer)) params.parallelism = payload.GetInteger("parallelism");

  switch (method) {
    case INFO:
      return invocation_result(true, json::encode({{ "threads", std::thread::hardware_concurrency() }}));
    case HASH:
      {
        auto t0 = std::chrono::high_resolution_clock::now();
        hashed = hash::pass::hash(payload.GetString("password"), salt, params);
        auto t1 = std::chrono::high_resolution_clock::now();
        hash_time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        return invocation_result(true, json::encode({
          { "hashed", hashed },
          { "params", params.toJSON() },
          { "durUS", hash_time.count() }
        }));
      }
    case SET:
      DBClient().putItem(
        hash::id::hash(payload.GetString("user"), PI_ID_SECRET), 
        hash::pass::hash(payload.GetString("password"), salt, params));
      return invocation_result(true, "");
    case CHECK:
    {
      auto item = DBClient().getItem(hash::id::hash(payload.GetString("user"), PI_ID_SECRET));
      if (!item) return invocation_result(false, "user doesn't exist");
      try {
        bool matches = hash::pass::check(payload.GetString("password"), item->hash, secret);
        return invocation_result(true, json::encode({{ "correct", matches }}));
      } catch (Argon2_ErrorCodes code) {
        return invocation_result(false, argon2_error_message(code));
      }
    }
    case EXISTS:
    {
      auto item = DBClient().getItem(hash::id::hash(payload.GetString("user"), PI_ID_SECRET));
      return invocation_result(true, json::encode({{ "exists", item != nullptr }}));
    }
    case DELETE:
      DBClient().deleteItem(hash::id::hash(payload.GetString("user"), PI_ID_SECRET));
      return invocation_result(true, "");
    default:
      return invocation_result(false, "unknown method");
  }
}
