#include "base32_rfc4648.hxx"
#include <cstddef>
#include <stdexcept>
#include <string_view>

std::string base32_encode(const std::vector<uint8_t> &data) {
  constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  constexpr char pad = '=';

  std::string result;

  if (data.empty())
    return result;

  uint8_t word_length = 5;
  uint8_t word = 0;
  // repr(word, "Word: ");

  for (const uint8_t &byte : data) {
    // repr(byte, "Input: ");
  }

  for (const uint8_t &byte : data) {
    uint8_t bits_remaining = 8;

    while (bits_remaining > 0) {
      const uint8_t value = byte << (8 - bits_remaining);

      if (bits_remaining >= word_length) {
        word |= value >> (8 - word_length);
        // repr(word, "Word: ");
        result.push_back(alphabet[word]);
        word = 0;
        bits_remaining -= word_length;
        word_length = 5;
      } else {
        word |= value >> (8 - bits_remaining);
        word_length -= bits_remaining;
        word <<= word_length;
        bits_remaining = 0;
      }
    }
  }

  if (0 < word_length and word_length < 5) {
    result.push_back(alphabet[word]);
  }

  while (result.size() % 8 != 0) {
    result.push_back(pad);
  }

  return result;
}

inline std::vector<uint8_t> b32decode(std::string_view data) {

  static constexpr std::string::value_type alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  static constexpr std::string::value_type padding_char = '=';

  std::vector<uint8_t> result;
  bool is_padding = false;

  if (data.length()) {
    uint8_t current, bits_needed = 8;

    for (auto value : data) {

      if (is_padding and value != padding_char)
        throw std::invalid_argument("b32decode: invalid padding schema");

      if (value == padding_char) {
        is_padding = true;
        continue;
      }

      value = std::toupper(value);

      auto pos = std::find(std::begin(alphabet), std::end(alphabet), value);

      if (pos == std::end(alphabet))
        throw std::invalid_argument(
            "b32decode: non base 32 character detected");
      size_t value_idx = pos - std::begin(alphabet);

      if (bits_needed > 4) {
        current |= value_idx;
        bits_needed -= 5;
        current <<= bits_needed;
      } else {
        current |= value_idx >> (5 - bits_needed);
        result.emplace_back(current);

        bits_needed += 3;
        current = value_idx << bits_needed;

        if (bits_needed > 5)
          current >>= bits_needed - 5;
      }
    }

    switch (bits_needed) {
    case 1:
    case 2:
    case 3:
      throw std::invalid_argument("b32decode: base32 string is corrupted");
    case 4:
    case 5:
    case 6:
    case 7:
      if (current != 0)
        throw std::invalid_argument("b32decode: base32 string is corrupted");
      break;
    case 8:
      break;
    default:
      __builtin_unreachable();
    }
  }

  return result;
}
