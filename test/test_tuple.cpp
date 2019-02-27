#include "emeaka_tuple.h"
#include "emeaka_intrinsics.h"
#include "catch.hpp"



TEST_CASE("A tuple with w=1.0 is a point","[tuple]")
{
    Tuple a(4.3f, -4.2f, 3.1f, 1.0f);
    REQUIRE(a.x == 4.3f);
    REQUIRE(a.y == -4.2f);
    REQUIRE(a.z == 3.1f);
    REQUIRE(a.w == 1.0f);
    REQUIRE(a.IsPoint());
    REQUIRE_FALSE(a.IsVector());
}

TEST_CASE("A tuple with w=0.0 is a vector","[tuple]")
{
    Tuple a(4.3f, -4.2f, 3.1f, 0.0f);
    REQUIRE(a.x == 4.3f);
    REQUIRE(a.y == -4.2f);
    REQUIRE(a.z == 3.1f);
    REQUIRE(a.w == 0.0f);
    REQUIRE_FALSE(a.IsPoint());
    REQUIRE(a.IsVector());
}

TEST_CASE("Point() creates tuples with w = 1.0","[tuple]")
{
    Tuple p = Point(4.f,-4.f,3.f);
    Tuple a(4.f,-4.f,3.f,1.f);
    REQUIRE(a==p);
}

TEST_CASE("Vector() creates tuples with w = 0.0","[tuple]")
{
    Tuple v = Vector(4.f,-4.f,3.f);
    Tuple a(4.f,-4.f,3.f,0.f);
    REQUIRE(a==v);
}

TEST_CASE("Adding two tuples","[tuple]")
{
    Tuple a(3.f,-2.f,5.f,1.f);
    Tuple b(-2.f,3.f,1.f,0.f);
    REQUIRE((a+b)==Tuple(1.f,1.f,6.f,1.f));
}

TEST_CASE("Subtracting two points","[tuple]")
{
    Tuple p1 = Point(3.f,2.f,1.f);
    Tuple p2 = Point(5.f,6.f,7.f);
    REQUIRE((p1-p2)==Vector(-2.f,-4.f,-6.f));
}

TEST_CASE("Subtracting vector a from a point", "[tuple]")
{
    Tuple p = Point(3.f,2.f,1.f);
    Tuple v = Vector(5.f,6.f,7.f);
    REQUIRE((p-v)==Point(-2.f,-4.f,-6.f));
}

TEST_CASE("Subtracting two vectors","[tuple]")
{
    Tuple v1 = Vector(3.f,2.f,1.f);
    Tuple v2 = Vector(5.f,6.f,7.f);
    REQUIRE((v1-v2)==Vector(-2.f,-4.f,-6.f));
}

TEST_CASE("Subtracting a vector from the zero vector","[tuple]")
{
    Tuple zero = Vector(0.f,0.f,0.f);
    Tuple v = Vector(1.f,-2.f,3.f);
    REQUIRE((zero-v)==Vector(-1.f,2.f,-3.f));
}

TEST_CASE("Negating a tuple","[tuple]")
{
    Tuple a(1.f,-2.f,3.f,-4.f);
    REQUIRE(-a == Tuple(-1.f,2.f,-3.f,4.f));
}
    
TEST_CASE("Multiplying a tuple by a scalar","[tuple]")
{
    Tuple a(1,-2,3,-4);
    REQUIRE((a*3.5f) == Tuple(3.5, -7, 10.5, -14));
}

TEST_CASE("Multiplying a tuple by a fraction","[tuple]")
{
    Tuple a(1,-2,3,-4);
    REQUIRE((a*0.5f) == Tuple(0.5,-1,1.5,-2));
}

TEST_CASE("Dividing a tuple by a scalar","[tuple]")
{
    Tuple a(1,-2,3,-4);
    REQUIRE((a/2)==Tuple(0.5,-1,1.5,-2));
}

TEST_CASE("Calculating unit magnitudes", "[tuple]")
{
    Tuple v1 = Vector(1,0,0);
    Tuple v2 = Vector(0,1,0);
    Tuple v3 = Vector(0,0,1);

    REQUIRE(v1.Length() == 1.0f);
    REQUIRE(v2.Length() == 1.0f);
    REQUIRE(v3.Length() == 1.0f);
}

TEST_CASE("Calculating complex magnitudes", "[tuple]")
{
    Tuple v1 = Vector(1,2,3);
    Tuple v2 = Vector(-1,-2,-3);
    REQUIRE(v1.Length() == Sqrt(14));
    REQUIRE(v2.Length() == Sqrt(14));
}

TEST_CASE("Normalizing simple vectors", "[tuple]")
{
    Tuple v1 = Vector(4,0,0);
    Tuple v2 = Vector(0,4,0);
    Tuple v3 = Vector(0,0,4);

    v1.Normalize();
    v2.Normalize();
    v3.Normalize();

    REQUIRE(v1 == Vector(1,0,0));
    REQUIRE(v2 == Vector(0,1,0));
    REQUIRE(v3 == Vector(0,0,1));
}

TEST_CASE("Normalizing complex vectors","[tuple]")
{
    Tuple v = Vector(1,2,3);
    v.Normalize();
    REQUIRE(v == Vector(1/Sqrt(14),2/Sqrt(14), 3/Sqrt(14)));
}

TEST_CASE("Magnitude of normalized vector is 1","[tuple]")
{
    Tuple v = Vector(1,2,3);
    v.Normalize();
    REQUIRE(v.Length() == Approx(1.0f));
}

TEST_CASE("Dot product of 2 vectors","[tuple]")
{
    Tuple v1 = Vector(1,2,3);
    Tuple v2 = Vector(2,3,4);
    REQUIRE(Dot(v1,v2) == 20.f);
}

TEST_CASE("Cross product of 2 vectors","[tuple]")
{
    Tuple v1 = Vector(1,2,3);
    Tuple v2 = Vector(2,3,4);

    REQUIRE(Cross(v1,v2) == Vector(-1,2,-1));
    REQUIRE(Cross(v2,v1) == Vector(1,-2,1));
}