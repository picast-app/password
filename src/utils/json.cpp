#include "json.h"

bool isType(JsonView &json, const Aws::String& key, JsonType type) {
  if (!json.ValueExists(key)) return false;
  switch (type) {
    case String: return json.GetObject(key).IsString();
    case Integer: return json.GetObject(key).IsIntegerType();
    default: return false;
  }
}
