#include <algorithm>
#include <bitset>
#include <climits>
#include <codecvt>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <ranges>
#include <string>
#include <type_traits>

void print_table(std::vector<std::vector<std::string>> const &table) {
  return;
  size_t col_max_width = 0, col_width = 0, columns = 0;

  for (auto const &row : table) {
    for (auto const &cell : row)
      col_max_width = std::max(col_max_width, cell.size());

    columns = std::max(columns, row.size());
  }

  // std::cout << "Columns: " << columns << std::endl;
  col_width = col_max_width * columns + (columns + 1) * 4;
  col_max_width += 2;

  for (auto i = 0; i < col_width; ++i)
    std::cout << "-";
  std::cout << std::endl;

  for (auto const &row : table) {
    for (auto const &cell : row)
      std::cout << std::left << std::setw(3) << "|" << std::setw(col_max_width)
                << cell;
    if (not row.empty())
      std::cout << "|";
    std::cout << std::endl;
  }

  for (auto i = 0; i < col_width; ++i)
    std::cout << "-";
  std::cout << std::endl;
}

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
  using DATA_TYPE = std::make_unsigned_t<std::ranges::range_value_t<RangeT>>;

  constexpr uint8_t DATA_TYPE_SIZE = sizeof(DATA_TYPE) * CHAR_BIT;
  constexpr uint8_t ENCODED_WORD_SIZE = CHAR_BIT * DerivedT::EncodedBitsPerByte;
  const uint8_t DATA_SIZE = DATA_TYPE_SIZE * data.size();

  uint8_t byte_encoded = 0;
  uint8_t bits_encoded = 0;
  uint8_t bytes_encoded = 0;
  std::vector<uint8_t> result;
  result.reserve(((DATA_SIZE / ENCODED_WORD_SIZE) +
                  (DATA_SIZE % ENCODED_WORD_SIZE == 0 ? 0 : 1)));

  std::vector<std::vector<std::string>> rows = {
      {"Type Size", "Data Size", "Codec Size"},
      {std::to_string((int)DATA_TYPE_SIZE), std::to_string((int)DATA_SIZE),
       std::to_string((int)ENCODED_WORD_SIZE)}};

  print_table(rows);
  rows.clear();
  rows.push_back({"Value", "Bits Remaining", "Bits Encoded", "LShifted",
                  "LSVal", "RShifted", "RSVal", "Outcome", "Added"});
  for (const auto &value : data) {
    uint8_t bits_remaining = DATA_TYPE_SIZE;

    while (bits_remaining > 0) {
      DATA_TYPE lshifted = (DATA_TYPE)(value)
                           << (DATA_TYPE_SIZE - bits_remaining);
      DATA_TYPE rshifted =
          lshifted >>
          (DATA_TYPE_SIZE - DerivedT::EncodedBitsPerByte + bits_encoded);
      byte_encoded |= (uint8_t)rshifted;

      rows.push_back(
          {std::bitset<DATA_TYPE_SIZE>(value).to_string(),
           std::to_string((int)bits_remaining),
           std::to_string((int)bits_encoded),
           std::to_string((int)DATA_TYPE_SIZE - bits_remaining),
           std::bitset<DATA_TYPE_SIZE>(lshifted).to_string(),
           std::to_string((int)DATA_TYPE_SIZE - bits_remaining + bits_encoded),
           std::bitset<DATA_TYPE_SIZE>(rshifted).to_string(),
           std::bitset<8>(byte_encoded).to_string()});

      if (bits_remaining < DerivedT::EncodedBitsPerByte) {
        bits_encoded = bits_remaining;
        bits_remaining = 0;
        rows.back().push_back("N");
      } else {
        rows.back().push_back("Y");
        result.push_back(DerivedT::ALPHABET[byte_encoded]);
        bytes_encoded++;
        bits_remaining -= DerivedT::EncodedBitsPerByte - bits_encoded;
        bits_encoded = 0;
        byte_encoded = 0;
      }
    }
  }

  if (bits_encoded > 0) {
    rows.back().pop_back();
    rows.back().push_back("Y");
    result.push_back(DerivedT::ALPHABET[byte_encoded]);
    bytes_encoded++;
  }

  print_table(rows);

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
  std::vector<uint8_t> result;
  switch (base) {
  case 32:
    result = encode<Base32>(value);
    break;
  case 64:
    result = encode<Base64>(value);
    break;
  }

  std::cout << "Base" << base << "Encoded: <" << value << "> = ";

  for (const auto &chr : result)
    std::cout << (char)chr;
  std::cout << std::endl;
}

template <typename T>
void encode(const auto &base,
            const T &value) requires(!std::same_as<T, std::string>) {
  std::wstring_convert<std::codecvt_utf8<typename T::value_type>,
                       typename T::value_type>
      converter;
  std::string converted = converter.to_bytes(value);
  std::vector<uint8_t> result;
  switch (base) {
  case 32:
    result = encode<Base32>(value);
    break;
  case 64:
    result = encode<Base64>(value);
    break;
  }
  std::cout << "Base" << base << "Encoded: <" << converted << "> = ";
  for (const auto &chr : result)
    std::cout << (char)chr;
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
