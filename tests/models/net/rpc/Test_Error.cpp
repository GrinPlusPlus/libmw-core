#include <catch.hpp>

#include <mw/core/models/net/rpc/Error.h>

TEST_CASE("RPC::Error::FromJSON")
{
    {
        json json({
            {"code", 100},
            {"message", "error_message"}
        });

        RPC::Error error = RPC::Error::FromJSON(json);
        REQUIRE(error.GetCode() == 100);
        REQUIRE(error.GetMsg() == "error_message");
        REQUIRE(!error.GetData().has_value());
    }

    {
        json data({ "field1", "test" });
        json json({
            {"code", 100},
            {"message", "error_message"},
            {"data", data}
        });

        RPC::Error error = RPC::Error::FromJSON(json);
        REQUIRE(error.GetCode() == 100);
        REQUIRE(error.GetMsg() == "error_message");
        REQUIRE(error.GetData().has_value());
        REQUIRE(error.GetData().value() == data);
    }

    {
        json json({
            {"code", 100},
            {"message", 500}
        });

        REQUIRE_THROWS_AS(RPC::Error::FromJSON(json), NetworkException);
    }

    {
        json json({
            {"code", "string"},
            {"message", "error_message"}
        });

        REQUIRE_THROWS_AS(RPC::Error::FromJSON(json), NetworkException);
    }
}

TEST_CASE("RPC::Error::ToJSON")
{
    {
        json data({ "field1", "test" });
        RPC::Error error(100, "error_message", tl::make_optional(data));

        REQUIRE(error.GetCode() == 100);
        REQUIRE(error.GetMsg() == "error_message");
        REQUIRE(error.GetData().has_value());
        REQUIRE(error.GetData().value() == data);
        
        json json({
            {"code", 100},
            {"message", "error_message"},
            {"data", data}
        });
        REQUIRE(error.ToJSON() == json);
    }
}