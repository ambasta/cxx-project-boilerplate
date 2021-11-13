#include "base32_rfc4648.hxx"
#include <iostream>

template <std::ranges::input_range ValueT>
void rangetest(const ValueT &values) {
  for (const auto &value : values) {
    std::cout << "Value: " << value << std::endl;
  }
}

template <typename T>
concept TConcept = std::ranges::input_range<typename T::alphabet>;

class Blah {
public:
  static char alphabet[];
};

char Blah::alphabet[] = "PQRST";

template <TConcept TT> class BlahWrap {
public:
  void show() {
    for (const auto &value : TT::alphabet) {
      std::cout << "Value: " << value << std::endl;
    }
  }
};

int main() {
  const char alphabet[] = "ABCD";
  rangetest(alphabet);
  // BlahWrap<Blah> bw;
  // bw.show();
  return 0;
}
