#include <codecvt>
#include <iostream>
#include <locale>

auto s2 = "\nHello\n  World\n";
auto s3 = "\nHello\n  World\n";
auto s4 = L"ABCDEF";
auto s5 = L"ABCDEF";
auto s6 = U"GHIJKL";
auto s7 = U"GHIJKL";
auto s9 = u"MNOPQR";
auto sA = u"MNOPQR";

const wchar_t *sC = LR"--(STUV)--"; // ok, raw string literal

template <typename T> void show(const T &value) {
  std::wstring_convert<std::codecvt_utf8<T>, T> converter;
  std::string s_value = converter.to_bytes(value);
  std::cout << s_value;
  std::cout << std::endl;
}
int main() {
  show(s4);
  show(s5);
  show(s6);
  show(s7);
  show(s9);
  show(sA);
  show(sC);
  return 0;
}
