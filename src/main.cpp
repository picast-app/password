#include <aws/core/Aws.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/lambda-runtime/runtime.h>
#include "hash.h"

using namespace aws::lambda_runtime;
using namespace Aws::Utils::Json;

bool isString(JsonView &json, const Aws::String &key)
{
  return json.ValueExists(key) && json.GetObject(key).IsString();
}

invocation_response handler(invocation_request const &request)
{
  JsonValue json(request.payload);
  if (!json.WasParseSuccessful())
    return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");

  auto payload = json.View();
  if (!isString(payload, "method"))
    return invocation_response::failure("Missing method", "InvalidJSON");

  std::string response = "";
  std::string error = "";

  if (payload.GetString("method") == "hash") {
    if (!isString(payload, "password")) {
      error = "missing password";
    } else {
      auto password = payload.GetString("password");
      try {
        response = hash(password.c_str());
      } catch (const char* err) {
        response = err;
        error = "EncodingFailed";
      }
    }
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
