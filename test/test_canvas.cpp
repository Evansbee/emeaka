#include "catch.hpp"
#include "rt_canvas.h"

TEST_CASE("Create a Canvas","[canvas]")
{
    Canvas c(10,20);
    REQUIRE(c.Width == 10);
    REQUIRE(c.Height == 20);
    for(auto y = 0; y < c.Height; ++y)
    {
        for(auto x = 0; x < c.Width; ++x)
        {
            if(c.Read(x,y) != Color(0,0,0)) FAIL();
        }
    }
    SUCCEED("All Colors Color(0,0,0)");
    
}

TEST_CASE("Writing pixels to Canvas","[canvas]")
{
    Canvas c(10,20);
    Color red(1,0,0);
    c.Write(2,3,red);
    REQUIRE(c.Read(2,3) == red);
}