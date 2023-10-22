#include <catch2/catch_test_macros.hpp>

#include <ecmascript/json/json.hpp>

TEST_CASE("null", "[json]")
{
    REQUIRE(parse_json("null").is_null());
}