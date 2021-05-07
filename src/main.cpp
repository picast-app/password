#include <aws/lambda-runtime/runtime.h>

using namespace aws::lambda_runtime;

invocation_response handler(invocation_request const &request)
{
  return invocation_response::success(request.payload, "application/json");
}

int main()
{
  run_handler(handler);
  return 0;
}
