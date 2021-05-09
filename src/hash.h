#ifndef PI_HASH_H_
#define PI_HASH_H_

#include <string>
#include <cstring>
#include <stdint.h>
#include "config.h"
#include "../argon2/include/argon2.h"

struct Argon2Params {
  uint32_t time_cost = 2;
  uint32_t memory_cost = 1 << 16;
  uint32_t parallelism = 1;
  
  std::string toJSON() {
    return "{\"time_cost\":" + std::to_string(time_cost) + 
            ",\"memory_cost\":" + std::to_string(memory_cost) +
            ",\"parallelism\":" + std::to_string(parallelism) + "}";
  }
};

std::string hash(const char* password, const uint8_t* salt, const Argon2Params params = {});

#endif
