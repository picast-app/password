#pragma once

#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <aws/core/Aws.h>
#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include "hash.h"
#include "utils/json.h"
#include "db.h"
#include "salt.h"

using namespace aws::lambda_runtime;

typedef enum {
  HASH,
  CHECK,
  SET,
  EXISTS,
  DELETE,
  INFO,
  UNKNOWN = -1
} method_code;

struct invocation_result {
  invocation_result() {};
  invocation_result(bool success, std::string msg, std::string code = ""): 
    success(success), response(msg), error(code) {};
  
  bool success = true;
  std::string error;
  std::string response;
};

invocation_result handleRequest(std::string method_name, Aws::Utils::Json::JsonView& payload, const uint8_t* const salt, std::string secret);
