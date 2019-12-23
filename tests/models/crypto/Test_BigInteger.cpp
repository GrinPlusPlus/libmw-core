#include <catch.hpp>

#include <mw/models/crypto/BigInteger.h>

// TODO: Implement

TEST_CASE("BigInt - Constructors")
{

}

TEST_CASE("BigInt - Hex")
{
    BigInt<8> bigInt1 = BigInt<8>::FromHex("0123456789AbCdEf");
    REQUIRE(bigInt1.ToHex() == "0123456789abcdef");
}