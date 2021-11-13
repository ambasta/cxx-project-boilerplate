#ifndef CODEX_BASE32_RFC4648
#define CODEX_BASE32_RFC4648

#include "base_n.hxx"
#include "concepts.hxx"
#include "properties.hxx"

class Base32RFC4648 : public BaseNEncoder<Base32RFC4648> {
public:
  static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  static constexpr Padding padding_policy = Padding::required;
  static constexpr char padding = '=';
  static constexpr uint8_t word_size = 8;
  static constexpr uint8_t encoding_bits = 5;

  /*
  static std::string_view encode(const std::vector<uint8_t> &);

  static std::string_view encode(const uint8_t *, size_t);

  static std::vector<uint8_t> decode(std::string_view);
  */

  static_assert(EncoderConcept<Base32RFC4648>);
};

class Base64RFC4648 : public BaseNEncoder<Base64RFC4648> {
public:
  static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static constexpr Padding padding_policy = Padding::required;
  static constexpr char padding = '=';
  static constexpr uint8_t word_size = 4;
  static constexpr uint8_t encoding_bits = 6;

  /*
  static std::string_view encode(const std::vector<uint8_t> &);
  */
};

auto base32_encode(const std::vector<uint8_t> &data) -> std::string;

auto b32decode(std::string_view data) -> std::vector<uint8_t>;

#endif
