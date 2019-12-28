#include <catch.hpp>

#include <mw/models/net/IPAddress.h>

TEST_CASE("IPAddress")
{
    REQUIRE(IPAddress::Parse("127.0.0.1").IsLocalhost() == true);
}