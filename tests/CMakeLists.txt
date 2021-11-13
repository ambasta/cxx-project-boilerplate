find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME}tests otp.cxx)
target_link_libraries(
  ${PROJECT_NAME}tests
  PUBLIC
  lib${PROJECT_NAME}
  ${PROJECT_NAME}_headers
  PRIVATE
  Catch2::Catch2
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}tests)
