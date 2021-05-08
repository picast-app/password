#include "salt.h"

const int part_len[5] = {8,4,4,4,12};

bool isUUID(const char* uuid) {
  int group = 0;
  int gi = 0;
  for (int i = 0; i < strlen(uuid); i++) {
    char c = uuid[i];
    if (c == '-') {
      if (gi != part_len[group]) return false;
      group++;
      if (group >= sizeof(part_len) / sizeof(int)) return false;
      gi = 0;
      continue;
    }
    if (!(c >= 'a' && c <= 'f' || c >= '0' && c <= '9')) return false;
    gi++;
  }
  if (group < sizeof(part_len) / sizeof(int) - 1 || gi != part_len[group]) return false;
  return true;
}

bool isUUIDv4(const char* uuid) {
  return isUUID(uuid) && uuid[14] == '4';
}

// technically a uuid v4 has 6-7 predetermined bits, 
// but 121 random bits is good enough for me
bool saltFromUUIDv4(const char *uuid, uint8_t *salt) {
  if (!isUUIDv4(uuid)) return false;
  int b = 0;
  for (int i = 1; i < strlen(uuid) && b < SALTLEN; i+=2) {
    if (uuid[i-1] == '-') i++;
    char hex[2] = {uuid[i-1], uuid[i]};
    salt[b++] = (uint8_t) strtoul(hex, nullptr, 16);
  }
  return b == SALTLEN;
}
