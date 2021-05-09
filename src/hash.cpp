#include "./hash.h"

std::string hash(const char* password, const uint8_t* salt, const Argon2Params params)
{
  uint32_t pass_len = strlen(password);
  uint8_t outhash[HASHLEN];

  argon2_context context = {
    .out          = outhash,
    .outlen       = HASHLEN,
    .pwd          = (uint8_t*)password,
    .pwdlen       = pass_len,
    .salt         = (uint8_t*)salt,
    .saltlen      = SALTLEN,
    .secret       = nullptr,
    .secretlen    = 0,
    .ad           = nullptr,
    .adlen        = 0, 
    .t_cost       = params.time_cost,
    .m_cost       = params.memory_cost,
    .lanes        = params.parallelism,
    .threads      = params.parallelism,
    .version      = ARGON2_VERSION_13,
    .allocate_cbk = nullptr,
    .free_cbk     = nullptr,
    .flags        = ARGON2_DEFAULT_FLAGS
  };

  auto status = argon2id_ctx(&context);
  if (status != ARGON2_OK) throw argon2_error_message(status);

  size_t enc_len = argon2_encodedlen(
    params.time_cost,
    params.memory_cost,
    params.parallelism,
    SALTLEN,
    HASHLEN,
    Argon2_id
  );
  char encoded[enc_len];
  encode_string(&encoded[0], enc_len, &context, Argon2_id);

  return std::string(encoded);
}
