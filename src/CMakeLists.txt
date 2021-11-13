# include(FindPkgConfig)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GLIB_DEPS REQUIRED IMPORTED_TARGET glibmm-2.68)

add_library(
  lib${PROJECT_NAME}
  algorithm.cxx
  otp.cxx
)

target_link_libraries(
  lib${PROJECT_NAME}
  ${PROJECT_NAME}_headers
  # PkgConfig::GLIB_DEPS
)
