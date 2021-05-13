#include <string>
#include <assert.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/lambda-runtime/runtime.h>
#include "config.h"
#include "salt.h"
#include "utils/json.h"
#include "handler.h"

using namespace aws::lambda_runtime;

invocation_response handlerResponse(invocation_request const &request, invocation_result const &result) {
  if (!result.success) return invocation_response::failure(result.response, result.error);
  else return invocation_response::success(result.response, "application/json");
}

invocation_response handler(invocation_request const &request)
{
  assert(strcmp(PI_SECRET, "") != 0 && strcmp(PI_ID_SECRET, "") != 0 && strcmp(AUTH_TOKEN, "") != 0);
  
  invocation_result result;
  JsonValue json(Aws::String(request.payload));

  auto error = [&](std::string msg, std::string code = "") {
    return handlerResponse(request, invocation_result(false, msg, code));
  };

  if (!json.WasParseSuccessful()) return error("failed to parse json", "InvalidJSON");
  auto payload = json.View();
  if (!isType(payload, "method", String)) return error("missing method");
  if (!isType(payload, "auth", String) || payload.GetString("auth") != AUTH_TOKEN)
    return error(payload.KeyExists("auth") ? "incorrect auth token" : "missing auth token", "AuthenticationError");
  uint8_t salt[SALTLEN];
  if (!saltFromUUIDv4(request.request_id.c_str(), salt)) return error("couldn't generate salt");
  
  Aws::SDKOptions sdk_opts;
  sdk_opts.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
  Aws::InitAPI(sdk_opts);

  try {
    result = handleRequest(payload.GetString("method"), payload, salt, PI_SECRET);
  } catch (...) {
    result.success = false;
    result.error = "internal error";
  }

  Aws::ShutdownAPI(sdk_opts);
  return handlerResponse(request, result);
}

int main()
{
  run_handler(handler);
  return 0;
}
