#pragma once

#include <string>
#include <cstring>
#include <stdint.h>
#include <thread>
#include <memory>
#include <sstream>
#include <exception>
#include "config.h"
#include "utils/json.h"

extern "C" {
  #include "../argon2/include/argon2.h"
  #include "../argon2/src/encoding.h"
  #include "../argon2/src/blake2/blake2.h"
}

struct Argon2Params {
  uint32_t time_cost = TIME_COST;
  uint32_t memory_cost = MEMORY_COST;
  uint32_t parallelism = PARALLELISM;
  const char* secret = nullptr;
};

namespace hash::pass {

std::string hash(std::string password, const uint8_t* salt, Argon2Params params = {});

bool check(std::string password, std::string encoded, std::string secret);

}

namespace hash::id {
  std::string hash(std::string id, std::string secret);
}
