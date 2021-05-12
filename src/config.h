#ifndef PI_CONFIG_H_
#define PI_CONFIG_H_

#include <stdlib.h>

#ifndef PI_SECRET
  #define PI_SECRET ""
#endif

#ifndef AUTH_TOKEN
  #define AUTH_TOKEN ""
#endif

const int TIME_COST   = 3;
const int PARALLELISM = 2;
const int MEMORY_COST = 25 * 1024;

const size_t HASHLEN  = 32;
const size_t SALTLEN  = 16;

#endif
