#ifndef CODEX_BASE_N
#define CODEX_BASE_N

#include "concepts.hxx"
#include "properties.hxx"
#include <codecvt>
#include <locale>
#include <span>

template <typename DerivedT> class BaseNEncoder {
public:
  template <typename T> static std::string encode(const T *, const size_t);

  // static std::string_view encode(const uint8_t *, const size_t);

  template <std::ranges::input_range RangeT>
  static auto encode(const RangeT &data) -> std::vector<uint8_t> {
    using DATA_TYPE = std::ranges::range_value_t<RangeT>;
    constexpr uint8_t DATA_TYPE_SIZE = sizeof(DATA_TYPE);
    constexpr uint8_t ENCODED_BIT_SIZE =
        DerivedT::BitsPerByte * DerivedT::EncodedBitsPerByte;
    constexpr uint8_t DATA_SIZE = DATA_TYPE_SIZE * data.size();

    // std::wstring_convert<std::codecvt_utf8<DATA_TYPE>, DATA_TYPE> converter;
    uint8_t byte_encoded = 0;
    uint8_t bits_encoded = 0;
    uint8_t bytes_encoded = 0;
    std::vector<uint8_t> result;
    result.reserve(
        ((DATA_SIZE / ENCODED_BIT_SIZE) + DATA_SIZE % ENCODED_BIT_SIZE == 0
             ? 0
             : 1) *
        DerivedT::BitsPerByte);

    for (const auto &value : data) {
      // const auto converted = converter.to_bytes(value);
      uint8_t bits_remaining = DATA_TYPE_SIZE;

      while (bits_remaining > 0) {
        byte_encoded |= (value << (DATA_TYPE_SIZE - bits_remaining)) >>
                        (DATA_TYPE_SIZE - DerivedT::BitsPerByte + bits_encoded);

        if (bits_remaining < DerivedT::BitsPerByte) {
          bits_encoded = bits_remaining;
          bits_remaining = 0;
        } else {
          result.push_back(DerivedT::ALPHABET[byte_encoded]);
          bytes_encoded++;
          bits_encoded = 0;
          byte_encoded = 0;
          bits_remaining -= DerivedT::BitsPerByte;
        }
      }
    }

    if (bits_encoded > 0) {
      result.push_back(DerivedT::ALPHABET[byte_encoded]);
      bytes_encoded++;
    }

    while (bytes_encoded % ENCODED_BIT_SIZE != 0) {
      result.push_back(DerivedT::PADDING);
      bytes_encoded++;
    }

    return result;
  }

  static std::vector<uint8_t> decode(const std::string_view &);
};

template <typename DerivedT>
template <typename T>
std::string BaseNEncoder<DerivedT>::encode(const T *data, const size_t size) {
  std::string encoded{""};
  constexpr uint8_t size_data = sizeof(T);
  std::wstring_convert<std::codecvt_utf8<T>, T> converter;
  std::string bytes = converter.to_bytes(reinterpret_cast<const T *>(data));
  uint8_t encoded_byte{0};
  uint8_t bits_encoded{0};
  uint8_t value{0};

  for (const auto &byte : std::span{data, size}) {
    uint8_t bits_left{size_data};

    while (bits_left > 0) {
      value |= (byte << (size_data - bits_left)) >>
               (size_data - DerivedT::word_size + bits_encoded);

      if (bits_left < DerivedT::word_size) {
        bits_encoded = bits_left;
        bits_left = 0;
      } else {
        encoded.push_back(DerivedT::alphabet[value]);
        bits_encoded = 0;
        bits_left -= DerivedT::word_size;
        value = 0;
      }
    }
  }

  if (bits_encoded > 0)
    encoded.push_back(DerivedT::alphabet[value]);

  // multiple of 4 for b64
  // multiple of 8 for b32
  // for (int i = size(encoded) % DerivedT::
  return encoded;
}

// decode
// std::wstring_convert<std::codecvt_utf8<T>, T> converter;
// auto as_integer = converter.from_bytes(decoded);
// return reinterpret_cast<const T*>(as_integer.data());

#endif
