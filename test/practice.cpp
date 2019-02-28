#include "catch.hpp"
#include "rt_canvas.h"
#include "emeaka_tuple.h"

#include <cmath>


TEST_CASE("Draw Cirlce","[raytracer]")
{
    Canvas c(800,800);
    auto Middle = Point(400,400,0);
    float r = 300;
    Color DrawColor(1,1,0);
    for(float a = 0; a < 2.f * M_PI; a += (2*M_PI)/2000.f)
    {
        float x = Middle.x + r * cosf(a);
        float y = Middle.y + r * sinf(a);
        DrawColor.r = (float)y / (float)c.Height;
        DrawColor.g = (float)y / (float)c.Height;
        DrawColor.b = 1 - (float)y / (float)c.Height;
        c.Write((int)x,(int)y,DrawColor);

        float nr = r * 0.75;
        x =  Middle.x + nr * cosf(a);
        y =  Middle.y + nr * sinf(a);
        DrawColor.r = (float)x / (float)c.Height;
        DrawColor.g = 1 - (float)x / (float)c.Height;
        DrawColor.b = (float)x / (float)c.Height;
        c.Write((int)x,(int)y,DrawColor);

        nr = r * 0.5;
        x =  Middle.x + nr * cosf(a);
        y =  Middle.y + nr * sinf(a);
        DrawColor.r = 1 - (float)x / (float)c.Height;
        DrawColor.g = (float)x / (float)c.Height;
        DrawColor.b = (float)y / (float)c.Height;
        c.Write((int)x,(int)y,DrawColor);
    }
    c.SaveHDR("DrawCircle.hdr");
}