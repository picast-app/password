#include "./hash.h"

std::string hash(const char* password, const uint8_t* salt)
{
  uint32_t t_cost = 2;         // 2-pass computation
  uint32_t m_cost = (1 << 16); // 64 mebibytes memory usage
  uint32_t parallelism = 1;    // number of threads and lanes

  size_t pass_len = strlen(password);

  size_t enclen = argon2_encodedlen(
    t_cost, 
    m_cost, 
    parallelism,
    SALTLEN, 
    HASHLEN, 
    Argon2_id
  );

  char enc[enclen];
  auto status = argon2id_hash_encoded(
    t_cost, 
    m_cost, 
    parallelism, 
    password, 
    pass_len, 
    salt,
    SALTLEN, 
    HASHLEN,
    enc, 
    enclen
  );

  if (ARGON2_OK != status) throw argon2_error_message(status);
  return std::string(enc);
}
