#include "otp.hxx"
#include <catch2/catch.hpp>
#include <locale>
#include <vector>
#include <cstdint>

TEST_CASE( "otp" ) { // init
    std::vector<uint32_t> data( 64, 0 );
    std::string secret_sha1{ b32encode( u8"12345678901234567890", data ) };
    std::string secret_sha256 { b32encode(u8"12345678901234567890123456789012", data) };
    std::string secret_sha512 { b32encode(u8"1234567890123456789012345678901234567890123456789012345678901234", data)};
    uint64_t counter1 = 59 / TOTP_DEFAULT_PERIOD;

    SECTION( "Test SHA1" ) {
        Algorithm algorithm{Algorithm::SHA1};
        REQUIRE(94287082 == htop(secret_sha1, counter1, algorithm, 8));
    }

    SECTION( "Test SHA256" ) {
        Algorithm algorithm{Algorithm::SHA256};
        REQUIRE(46119246 == hotp(secret_sha256, counter1, algorithm, 8));
    }

    SECTION( "Test SHA512" ) {
        Algorithm algorithm{Algorithm::SHA512};
        REQUIRE(90693936 == hotp(secret_sha512, counter1, algorithm, 8));
    }
}
