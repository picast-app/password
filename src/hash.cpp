#include "./hash.h"

const size_t SALTLEN = 16;
const size_t HASHLEN = 32;

std::string hash(const char *password)
{
  uint8_t salt[SALTLEN];
  memset(salt, 0x00, SALTLEN);

  uint32_t t_cost = 2;         // 2-pass computation
  uint32_t m_cost = (1 << 16); // 64 mebibytes memory usage
  uint32_t parallelism = 1;    // number of threads and lanes

  size_t enclen = argon2_encodedlen(t_cost, m_cost, parallelism, SALTLEN, HASHLEN, Argon2_id);
  char enc[enclen];
  auto status = argon2id_hash_encoded(t_cost, m_cost, parallelism, password, strlen(password), salt, SALTLEN, HASHLEN, enc, enclen);

  if (ARGON2_OK != status) throw argon2_error_message(status);
  return std::string(enc);
}
