#ifndef AUTHENTICATOR_OTP
#define AUTHENTICATOR_OTP

#include "algorithm.hxx"
#include "hmac.hxx"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <ranges>
#include <string>
#include <string_view>
#include <valarray>
#include <vector>

// TODO
// Use some codec to encode/decode
auto decode_secret(std::string_view secret) -> std::vector<uint8_t>;

// TODO
// Check if the secret can be decoded successfully
auto is_valid(std::string_view secret) -> bool;

// TODO
// Calculate the HMAC digest for secret and counter
auto calculate_digest(std::vector<uint8_t> &secret, uint64_t counter,
                      Algorithm &algorithm) -> hmac::Tag;

// TODO
// Encode digest into a n-digit integer
auto encode_digest(std::vector<uint8_t>* digest) -> uint32_t;

// TODO
// Performs the HMAC based OTP Algorithm
auto hotp(std::string_view secret, uint64_t counter, Algorithm &algorithm,
          uint32_t digits) -> uint32_t;

#endif
