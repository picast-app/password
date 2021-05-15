#include "json.h"

bool json::isType(JsonView& json, const Aws::String& key, JsonType type) {
  if (!json.ValueExists(key)) return false;
  switch (type) {
    case String: return json.GetObject(key).IsString();
    case Integer: return json.GetObject(key).IsIntegerType();
    default: return false;
  }
}

std::string json::Value::get() const {
  switch (which) {
    case STRING: return std::string("\"") + s + '"';
    case BOOL: return b ? "true" : "false";
    case NUM: { 
      auto str = std::to_string(n);
      if (str.find('.') == std::string::npos) return str;
      while (str[str.length() - 1] == '0') str.pop_back();
      if (str[str.length() - 1] == '.')  str.pop_back();
      return str;
    }
    default: return "null";
  }
}

std::string json::encode(std::vector<std::pair<std::string, Value>> input) {
  if (!input.size()) return "{}";
  std::string json = "";
  for (auto& [k, v] : input) json += ",\"" + k + "\":" + v.get();
  json[0] = '{';
  return json + '}';
}
