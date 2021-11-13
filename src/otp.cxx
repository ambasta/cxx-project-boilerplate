#include "otp.hxx"
#include "hmac.hxx"
#include <byteswap.h>
#include <chrono>
#include <concepts>
#include <range/v3/view/enumerate.hpp>
#include <ranges>

#if __cpp_lib_format >= 201907L
#include <format>
#else
#include <fmt/format.h>
namespace std {
using fmt::format;
}
#endif

#define STEAM_CHARS "23456789BCDFGHJKMNPQRTVWXY"
#define STEAM_DEFAULT_PERIOD 30
#define STEAM_DEFAULT_DIGITS 5
#define HOTP_DEFAULT_COUNTER 1
#define TOTP_DEFAULT_PERIOD 30

template <std::ranges::input_range V> std::vector<uint8_t> b32decode(V secret) {
  std::vector<uint8_t> output;
  for ([[maybe_unused]] auto const chr : secret)
    output.emplace_back(0);
  return output;
};

std::vector<uint8_t> decode_secret(std::string_view secret) {
  return b32decode(
      secret | std::views::filter([](char chr) { return chr != ' '; }) |
      std::views::transform([](char chr) { return std::toupper(chr); }));
}

bool is_valid(std::string_view secret) {
  try {
    decode_secret(secret);
  } catch (std::exception &exc) {
    return false;
  }
  return true;
}

hmac::Tag calculate_digest(std::vector<uint8_t> &secret, uint64_t counter,
                           Algorithm &algorithm) {
  auto byte_ptr = reinterpret_cast<std::byte *>(&counter);

  return hmac::sign(
      hmac::Key{secret, algorithm},
      std::vector<std::byte>{byte_ptr, byte_ptr + sizeof(uint64_t)});
}

uint32_t encode_digest(std::vector<uint8_t> *digest) {
  size_t offset = digest->back() & 0xf;
  std::vector<uint8_t> buffer(digest->begin() + offset,
                              digest->begin() + offset + 4);
  return (int)(buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]) &
         0x7fffffff;
}

uint32_t hotp(std::string_view secret, uint64_t counter, Algorithm &algorithm,
              uint32_t digits) {
  auto decoded_secret = decode_secret(secret);
  auto digest = encode_digest(
      calculate_digest(decoded_secret, counter, algorithm).as_ref());

  return digest % (unsigned int)pow((uint32_t)10, digits);
}

std::string steam(std::string secret, uint64_t counter) {
  auto decoded_secret = decode_secret(secret);
  Algorithm algorithm{Algorithm::SHA1};
  auto full_token = encode_digest(
      calculate_digest(decoded_secret, counter, algorithm).as_ref());
  std::string code;
  uint32_t total_chr = sizeof(STEAM_CHARS) / sizeof(STEAM_CHARS[0]);

  for (auto const idx :
       std::views::iota((uint32_t)0, (uint32_t)STEAM_DEFAULT_DIGITS)) {
    auto position = full_token % total_chr;
    auto chr = STEAM_CHARS[position];
    code.push_back(chr);
    full_token /= total_chr;
  }
  return code;
}

std::string format(uint32_t code, size_t digits) {
  auto width = digits;
  auto padded_code = std::format("{:<{}}", code, digits);
  std::string formatted_code;

  for (auto const &[idx, chr] : ranges::views::enumerate(padded_code)) {
    if ((digits - idx) % 3 == 0 and (digits - idx) != 0 and idx != 0)
      formatted_code.push_back(' ');
    formatted_code.push_back(chr);
  }
  return formatted_code;
}

uint64_t time_based_counter(uint32_t period) {
  auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
  return (uint64_t)(timestamp / period);
}
