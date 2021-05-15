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

  bool isType(JsonView& json, const Aws::String& key, JsonType type);

  struct Value {
    Value(const char* v): which(STRING), s(v) {};
    Value(const std::string& v): which(STRING), s(v.c_str()) {};
    Value(bool v): which(BOOL), b(v) {};
    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    Value(T v): which(NUM), n((double)v) {}
  
    const enum{STRING, BOOL, NUM} which;
    const union {
      bool b;
      const char* s;
      double n;
    };

    std::string get() const;
  };

  std::string encode(std::vector<std::pair<std::string, Value>> input);
  
}
