// enum class OTPMethod { TOTP = 0, HOTP = 1, Steam = 2 };
//
#ifndef AUTHENTICATOR_ALGORITHM
#define AUTHENTICATOR_ALGORITHM

#include <concepts>
#include <string>
#include <utility>
#include <vector>

namespace nostd {
namespace adl_helper {
template <class T> std::string as_string(T &&t) {
  using std::to_string;
  return to_string(std::forward<T>(t));
}
} // namespace adl_helper

template <class T> std::string to_string(T &&t) {
  return adl_helper::as_string(std::forward<T>(t));
}
} // namespace nostd

template <typename T>
concept SerializerConcept = requires(T t) {
  { t.serialize() } -> std::same_as<std::vector<std::byte>>;
  { t.template serialize() } -> std::same_as<std::vector<std::byte>>;
};

class OTPMethod {

public:
  enum Value { TOTP, HOTP, Steam };

private:
  Value m_value;

public:
  OTPMethod() = default;

  constexpr OTPMethod(Value value);

  constexpr OTPMethod(uint32_t);

  constexpr OTPMethod(std::string_view);

  constexpr bool operator==(OTPMethod) const;

  constexpr bool operator!=(OTPMethod) const;

  friend std::string to_string(OTPMethod const &);

  friend std::vector<std::byte> serialize(const OTPMethod &);

  friend bool deserialize(const std::byte *&, const std::byte *, OTPMethod &);

  std::string to_locale_string() const;
};

class Algorithm {
public:
  enum Value { SHA1, SHA256, SHA512 };

private:
  Value m_value;

public:
  Algorithm() = default;

  Algorithm(uint32_t);

  Algorithm(std::string_view);

  friend std::string to_string(Algorithm const &);

  friend std::vector<std::byte> serialize(const Algorithm &);

  friend bool deserialize(const std::byte *&, const std::byte *, Algorithm &);

  std::string to_locale_string() const;
};
#endif
