#ifndef AUTHENTICATOR_HMAC
#define AUTHENTICATOR_HMAC

#include <cstddef>
#include <cstdint>
#include <vector>

namespace hmac {
class Tag {
public:
  std::vector<uint8_t>* as_ref() const;
};

template <typename AlgorithmT> class Key {
public:
  Key() = default;

  Key(std::vector<uint8_t> &, const AlgorithmT &);
};

template <typename AlgorithmT>
Tag sign(Key<AlgorithmT>, const std::vector<std::byte> &);
}; // namespace hmac

#endif
