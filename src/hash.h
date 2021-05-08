#ifndef PI_HASH_H_
#define PI_HASH_H_

#include <string>
#include <cstring>
#include <stdint.h>
#include "config.h"
#include "../argon2/include/argon2.h"

std::string hash(const char* password, const uint8_t* salt);

#endif
