#ifndef HASH_H_
#define HASH_H_

#include <string>
#include <cstring>
#include "../argon2/include/argon2.h"

std::string hash(const char *password);

#endif
