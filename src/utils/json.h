#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <aws/core/utils/json/JsonSerializer.h>

using namespace Aws::Utils::Json;

typedef enum  {
  String,
  Integer
} JsonType;

namespace json {

bool isType(JsonView& json, const Aws::String& key, JsonType type) {
  if (!json.ValueExists(key)) return false;
  switch (type) {
    case String: return json.GetObject(key).IsString();
    case Integer: return json.GetObject(key).IsIntegerType();
    default: return false;
  }
}

struct Value {
  Value(const char* v): which(STRING), s(v) {};
  Value(std::string v): which(STRING), s(v.c_str()) {};
  Value(bool v): which(BOOL), b(v) {};
  template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  Value(T v): which(NUM), n((double)v) {}
  
  const enum{STRING, BOOL, NUM} which;
  const union {
    bool b;
    const char* s;
    double n;
  };

  std::string get() const {
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
};

std::string encode(std::vector<std::pair<std::string, Value>> input) {
  if (!input.size()) return "{}";
  std::string json = "";
  for (auto& [k, v] : input) json += ",\"" + k + "\":" + v.get();
  json[0] = '{';
  return json + '}';
}

}
