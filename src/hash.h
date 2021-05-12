#ifndef PI_HASH_H_
#define PI_HASH_H_

#include <string>
#include <cstring>
#include <stdint.h>
#include <thread>
#include <memory>
#include "config.h"

extern "C" {
  #include "../argon2/include/argon2.h"
  #include "../argon2/src/encoding.h"
}

struct Argon2Params {
  uint32_t time_cost = 3;
  uint32_t memory_cost = 25 * 1024;
  uint32_t parallelism = 2;
  const char* secret = nullptr;
  
  std::string toJSON() {
    return "{\"time_cost\":" + std::to_string(time_cost) + 
            ",\"memory_cost\":" + std::to_string(memory_cost) +
            ",\"parallelism\":" + std::to_string(parallelism) + "}";
  }
};

namespace pass {

std::string hash(std::string password, const uint8_t* salt, Argon2Params params = {});

bool check(std::string password, std::string encoded, std::string secret);

}

#endif
