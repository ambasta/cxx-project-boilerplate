#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

void repr(uint8_t value, std::string_view prefix) {
  std::cout << prefix << std::bitset<8>(value) << std::endl;
}

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

auto base32_encode(std::string_view data) -> std::string {
  std::vector<uint8_t> bytes;
  bytes.reserve(data.size());

  for (auto c : data) {
    bytes.push_back(c);
  }

  return base32_encode(bytes);
}

void test(std::string_view lhs, std::string_view rhs) {
  if (lhs != rhs) {
    std::cout << "FAIL: Got<" << lhs << "> != Expected<" << rhs << ">"
              << std::endl;
  }
}

int main() {
  using namespace std::string_literals;

  std::vector<std::string> decoded = {
      "", "a"s, "ab"s, "abc"s, "abcd"s, "abcde"s,
  };

  std::vector<std::string> encoded = {
      "", "ME======"s, "MFRA===="s, "MFRGG==="s, "MFRGGZA="s, "MFRGGZDF"s,
  };

  for (auto idx = 0u; idx < encoded.size(); ++idx)
    test(base32_encode(decoded[idx]), encoded[idx]);
  return 0;
}
