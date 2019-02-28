#include "catch.hpp"
#include "rt_canvas.h"

TEST_CASE("Test color construction","[color]")
{
    Color c(1.f, 0.5f, 0.25f);
    REQUIRE(c.r == 1.f);
    REQUIRE(c.g == 0.5f);
    REQUIRE(c.b == 0.25f);
}

TEST_CASE("Test color addition", "[color]")
{
    Color c1(1,2,3);
    Color c2(3,4,5);
    REQUIRE((c1+c2)==Color(4,6,8));
}

TEST_CASE("Test color subtraction", "[color]")
{
    Color c1(1,2,3);
    Color c2(3,4,5);
    REQUIRE((c1-c2)==Color(-2,-2,-2));
}

TEST_CASE("Test color multiplication (2 colors)", "[color]")
{
    Color c1(1,2,3);
    Color c2(3,4,5);
    REQUIRE((c1*c2)==Color(3,8,15));
}

TEST_CASE("Test color multiplication (color * scalar)", "[color]")
{
    Color c1(1,2,3);
    REQUIRE((c1 * 5)==Color(5,10,15));
    REQUIRE((5 * c1)==Color(5,10,15));
}