#include "algorithm.hxx"
#include <stdexcept>

std::string to_string(OTPMethod const &otp_method) {
  if (otp_method.m_value == OTPMethod::Value::TOTP)
    return "totp";

  if (otp_method.m_value == OTPMethod::Value::HOTP)
    return "hotp";
  return "steam";
}

template <typename T> std::vector<std::byte> serialize(T t) { return {}; }

template <typename T>
bool deserialize(const std::byte *&, const std::byte *, T &t) {
  return true;
}

std::vector<std::byte> serialize(const OTPMethod &otp_method) {
  auto ret = serialize(otp_method.m_value);
  return ret;
}

bool deserialize(const std::byte *&start, const std::byte *end,
                 OTPMethod &otp_method) {
  OTPMethod::Value value;

  if (not deserialize(start, end, value))
    return false;
  otp_method.m_value = std::move(value);
  return true;
}

Algorithm::Algorithm(uint32_t algorithm) {
  switch (algorithm) {
  case 0:
    this->m_value = Value::SHA1;
    break;
  case 1:
    this->m_value = Value::SHA256;
    break;
  case 2:
    this->m_value = Value::SHA512;
    break;
  default:
    throw std::runtime_error("Invalid algorithm selected");
  }
}
