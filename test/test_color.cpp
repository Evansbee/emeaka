#include "catch.hpp"
#include "rt_canvas.h"

TEST_CASE("Test color construction","[color]")
{
    Color c(1.f, 0.5f, 0.25f);
    REQUIRE(c.r == 1.f);
    REQUIRE(c.g == 0.5f);
    REQUIRE(c.b == 0.25f);
}

TEST_CASE("WTF","[color]")
{
    Canvas c(640,480);
    c.Write(10,10, Color(1.f,0,1.f)); 
    c.SaveHDR("tmp/HDROutputTest.hdr");
}