#include "./hash.h"

std::string hash(const char* password, const uint8_t* salt, const Argon2Params params)
{
  size_t pass_len = strlen(password);
  size_t enclen = argon2_encodedlen(
    params.time_cost,
    params.memory_cost,
    params.parallelism,
    SALTLEN,
    HASHLEN,
    Argon2_id
  );

  char enc[enclen];
  auto status = argon2id_hash_encoded(
    params.time_cost,
    params.memory_cost,
    params.parallelism,
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
