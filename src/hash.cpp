#include "./hash.h"

std::string hash(std::string password, const uint8_t* salt, const Argon2Params params)
{
  uint8_t outhash[HASHLEN];
  uint32_t pass_len = password.length();

  argon2_context context = {
    .out          = outhash,
    .outlen       = HASHLEN,
    .pwd          = (uint8_t*)password.c_str(),
    .pwdlen       = pass_len,
    .salt         = (uint8_t*)salt,
    .saltlen      = SALTLEN,
    .secret       = (uint8_t*)params.secret,
    .secretlen    = (uint32_t)(params.secret ? strlen(params.secret) : 0),
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
  if (encode_string(&encoded[0], enc_len, &context, Argon2_id) != ARGON2_OK) 
    throw "encoding failed";

  return std::string(encoded);
}
