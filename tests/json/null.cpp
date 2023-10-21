#include <catch2/catch_test_macros.hpp>

#include <ecmascript/json/json.hpp>

TEST_CASE("Null is parsed", "[json]")
{
    REQUIRE(parse_json("null").is_null());
}