#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include "config.h"

namespace salt {

bool isUUID(const char* uuid);
bool isUUIDv8(const char* uuid);
bool fromUUIDv4(const char* uuid, uint8_t* salt);

std::string toString(const uint8_t* const salt);
void fromString(const std::string& hex, uint8_t* salt);

}
