#include <algorithm>
#include <bitset>
#include <climits>
#include <codecvt>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <locale>
#include <ranges>
#include <string>
#include <type_traits>

/*
class Encoder {
private:
  constexpr static std::string::value_type m_alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  constexpr static std::string::value_type m_padding_char = '=';
  int16_t m_skip;
  int16_t m_bits;
  std::string m_output;

public:
  Encoder() : m_skip(0), m_bits(0), m_output("") {}

  int read_byte(uint8_t byte) {
    if (m_skip < 0)
      m_bits |= (byte >> (-m_skip));
    else
      m_bits = (byte << m_skip) & 248;

    if (m_skip > 3) {
      m_skip -= 8;
      return 1;
    }

    if (m_skip < 4) {
      m_output.push_back(m_alphabet[m_bits >> 3]);
      m_skip += 5;
    }

    return 0;
  }

  std::string finish(bool check = false) {
    std::string l_output = m_output;

    if (m_skip < 0)
      l_output.push_back(m_alphabet[m_bits >> 3]);

    if (check)
      l_output.push_back('$');
    m_output = "";
    return l_output;
  }

  std::string update(std::string_view data, bool flush) {
    for (const auto &byte : data) {
      read_byte(byte);
    }

    auto l_output = m_output;
    m_output = "";

    if (flush)
      l_output += finish();
    return l_output;
  }
};

class RFCEncoder {
private:
  constexpr static std::string::value_type m_alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  constexpr static std::string::value_type m_padding_char = '=';
  constexpr static uint8_t m_bits_per_char = 5;
  constexpr static uint8_t m_bitgroup_size = 40;

  // take 5 8-bit input groups
  // concatenate them into 40-bit groups
  // treat them as 8 5-bit groups contactenated together
  // translate each of the 8 5-bit groups into a base32 character from alphabet
  //
  // input groups are presumed to be ordered with MSB first
public:
  std::string encode(std::string_view data) {
    uint8_t bits_remaining = 40;
    std::string interim{""};
    std::string output{""};

    for (const uint8_t &chr : data) {
      std::string char_byte_repr = std::bitset<8>(chr).to_string();
      interim += char_byte_repr;
    }

    while (interim.length() % 40 != 0)
      interim += "X";

    for (uint idx = 0; idx < interim.size(); idx += 5) {
      std::string bitchar = interim.substr(idx, 5);

      if (bitchar.find('1') != std::string::npos and
          bitchar.find('X') != std::string::npos) {
        std::string newchar{""};

        for (auto const &chr : bitchar) {
          if (chr == 'X')
            newchar.push_back('0');
          else
            newchar.push_back(chr);
        }
        bitchar = newchar;
      }

      if (bitchar == "XXXXX") {
        output.push_back(m_padding_char);
      } else {
        uint8_t bitgroup = std::stoi(bitchar, nullptr, 2);
        char base32_char = m_alphabet[bitgroup];
        output.push_back(base32_char);
      }
    }
    return output;
  }
};
*/

struct Base32 {
  constexpr static std::string::value_type ALPHABET[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  constexpr static std::string::value_type PADDING = '=';
  constexpr static uint8_t EncodedBitsPerByte = 5;
  constexpr static uint8_t WORD_GROUP_SIZE = 8;
};

struct Base64 {
  constexpr static std::string::value_type ALPHABET[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  constexpr static std::string::value_type PADDING = '=';
  constexpr static uint8_t EncodedBitsPerByte = 6;
  constexpr static uint8_t WORD_GROUP_SIZE = 4;
};

template <typename DerivedT, std::ranges::input_range RangeT>
auto encode(const RangeT &data) -> std::vector<uint8_t> {
  using DATA_TYPE = std::ranges::range_value_t<RangeT>;

  constexpr uint8_t DATA_TYPE_SIZE = sizeof(DATA_TYPE) * CHAR_BIT;
  constexpr uint8_t ENCODED_WORD_SIZE = CHAR_BIT * DerivedT::EncodedBitsPerByte;
  const uint8_t DATA_SIZE = DATA_TYPE_SIZE * data.size();

  uint8_t byte_encoded = 0;
  uint8_t bits_encoded = 0;
  uint8_t bytes_encoded = 0;
  std::vector<uint8_t> result;
  result.reserve(((DATA_SIZE / ENCODED_WORD_SIZE) +
                  (DATA_SIZE % ENCODED_WORD_SIZE == 0 ? 0 : 1)));

  for (const auto &value : data) {
    uint8_t bits_remaining = DATA_TYPE_SIZE;

    while (bits_remaining > 0) {
      DATA_TYPE lshifted = value << (DATA_TYPE_SIZE - bits_remaining);
      DATA_TYPE rshifted =
          lshifted >>
          (DATA_TYPE_SIZE - DerivedT::EncodedBitsPerByte + bits_encoded);
      byte_encoded |= (uint8_t)rshifted;
      // (DATA_TYPE)(value << (DATA_TYPE_SIZE - bits_remaining)) >>
      // (DATA_TYPE_SIZE - DerivedT::EncodedBitsPerByte + bits_encoded);

      if (bits_remaining < DerivedT::EncodedBitsPerByte) {
        bits_encoded = bits_remaining;
        bits_remaining = 0;
      } else {
        result.push_back(DerivedT::ALPHABET[byte_encoded]);
        bytes_encoded++;
        bits_remaining -= DerivedT::EncodedBitsPerByte - bits_encoded;
        bits_encoded = 0;
        byte_encoded = 0;
      }
    }
  }

  if (bits_encoded > 0) {
    result.push_back(DerivedT::ALPHABET[byte_encoded]);
    bytes_encoded++;
  }

  while (bytes_encoded % DerivedT::WORD_GROUP_SIZE != 0) {
    result.push_back(DerivedT::PADDING);
    bytes_encoded++;
  }

  return result;
}

inline std::vector<uint8_t> b32decode(std::string_view data) {

  static constexpr std::string::value_type alphabet[] = "AAAEQNY=";
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

template <typename T>
void encode(const auto &base,
            const T &value) requires(std::same_as<T, std::string>) {
  std::cout << "Base" << base << "Encoded: <";
  std::cout << value;
  std::cout << "> = ";
  switch (base) {
  case 32:
    for (const auto &chr : encode<Base32>(value))
      std::cout << (char)chr;
    break;
  case 64:
    for (const auto &chr : encode<Base64>(value))
      std::cout << (char)chr;
    break;
  }
  std::cout << std::endl;
}

template <typename T>
void encode(const auto &base,
            const T &value) requires(!std::same_as<T, std::string>) {
  std::wstring_convert<std::codecvt_utf8<typename T::value_type>,
                       typename T::value_type>
      converter;
  std::string converted = converter.to_bytes(value);
  std::cout << "Base" << base << "Encoded: <";
  std::cout << converted;
  std::cout << "> = ";
  switch (base) {
  case 32:
    for (const auto &chr : encode<Base32>(value))
      std::cout << (char)chr;
    break;
  case 64:
    for (const auto &chr : encode<Base64>(value))
      std::cout << (char)chr;
    break;
  }
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  /*
  std::string encoded{"IFRGGZDFMY======"};

  auto decoded = b32decode(encoded);
  std::cout << "Decoded: ";

  for (auto value : decoded) {
    std::cout << (char)value;
  }

  std::cout << std::endl;
  */

  std::string input_string_utf8 = "AmiT";
  std::u16string input_string_utf16 = u"AmiT";
  std::u32string input_string_utf32 = U"AmiT";
  encode(32, input_string_utf8);
  encode(64, input_string_utf8);

  encode(32, input_string_utf16);
  encode(64, input_string_utf16);

  encode(32, input_string_utf32);
  encode(64, input_string_utf32);
  return 0;
}
