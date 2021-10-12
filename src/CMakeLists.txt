include(FindPkgConfig)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB_DEPS REQUIRED IMPORTED_TARGET glib-2.0)

add_library(libauthen database.cxx)
target_link_libraries(authenticator PkgConf::GLIB_DEPS)
