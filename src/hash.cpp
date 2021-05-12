#include "./hash.h"

namespace pass {

std::unique_ptr<argon2_context> makeContext(std::string password, Argon2Params& params) {
  auto ctx = std::make_unique<argon2_context>();

  ctx->pwd          = (uint8_t*)password.c_str();
  ctx->pwdlen       = password.length();
  ctx->secret       = (uint8_t*)params.secret;
  ctx->secretlen    = (uint32_t)(params.secret ? strlen(params.secret) : 0);
  ctx->ad           = nullptr;
  ctx->adlen        = 0;
  ctx->t_cost       = params.time_cost;
  ctx->m_cost       = params.memory_cost;
  ctx->lanes        = params.parallelism;
  ctx->threads      = params.parallelism;
  ctx->version      = ARGON2_VERSION_13;
  ctx->allocate_cbk = nullptr;
  ctx->free_cbk     = nullptr;
  ctx->flags        = ARGON2_DEFAULT_FLAGS;
  
  return ctx;
}

std::string hash(std::string password, const uint8_t* salt, Argon2Params params)
{
  uint8_t outhash[HASHLEN];
  uint32_t pass_len = password.length();

  auto ctx = makeContext(password, params);
  ctx->out      = outhash;
  ctx->outlen   = HASHLEN;
  ctx->salt     = (uint8_t*)salt;
  ctx->saltlen  = SALTLEN;

  auto status = argon2id_ctx(ctx.get());
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
  if (encode_string(&encoded[0], enc_len, ctx.get(), Argon2_id) != ARGON2_OK) 
    throw "encoding failed";

  return std::string(encoded);
}

bool check(std::string password, std::string encoded, std::string secret) {
  argon2_context ctx;
  uint32_t max_field_len = encoded.length();

  ctx.saltlen = max_field_len;
  ctx.outlen = max_field_len;
  ctx.salt = (uint8_t*)malloc(ctx.saltlen);
  ctx.out = (uint8_t*)malloc(ctx.outlen);

  if (!ctx.salt || !ctx.out) throw ARGON2_MEMORY_ALLOCATION_ERROR;
  
  ctx.pwd = (uint8_t*)password.c_str();
  ctx.pwdlen = password.length();

  if (decode_string(&ctx, encoded.c_str(), Argon2_id) != ARGON2_OK) 
    throw ARGON2_DECODING_FAIL;

  ctx.secret = (uint8_t*)secret.c_str();
  ctx.secretlen = secret.length();

  auto desired = ctx.out;
  ctx.out = (uint8_t*)malloc(ctx.outlen);

  auto status = argon2_verify_ctx(&ctx, (char*)desired, Argon2_id);
  if (status == ARGON2_OK) return true;
  if (status != ARGON2_VERIFY_MISMATCH) throw status;
  return false;
}

}
