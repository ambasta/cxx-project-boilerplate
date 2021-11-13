#ifndef CODEX_FORMAT
#define CODEX_FORMAT

#if __cpp_lib_format >= 202110L
#include <format>
#else
#include <fmt/format.h>
namespace std {
using fmt::format;
};
#endif

#endif
