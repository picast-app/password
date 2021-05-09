#ifndef PI_JSON_H_
#define PI_JSON_H_

#include <aws/core/utils/json/JsonSerializer.h>

using namespace Aws::Utils::Json;

typedef enum  {
  String,
  Integer
} JsonType;

bool isType(JsonView &json, const Aws::String& key, JsonType type);

#endif
