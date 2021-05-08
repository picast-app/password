#ifndef PI_SALT_H_
#define PI_SALT_H_

#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include "config.h"

bool isUUID(const char* uuid);
bool isUUIDv8(const char* uuid);
bool saltFromUUIDv4(const char* uuid, uint8_t* salt);

#endif
