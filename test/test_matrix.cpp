#include "catch.hpp"
#include "emeaka_matrix.h"
#include <iostream>

TEST_CASE("Matrix wtf","[matrix]")
{
    Matrix4f m;
    m.d[0][0] = 1.0f;
    m.d[1][3] = 2.0f;
    REQUIRE(m[0][0] == 1.0f);
    REQUIRE(m[1][3] == 2.0f);
}

TEST_CASE("Matrix can be equal","[matrix]")
{
    Matrix4f m1;
    Matrix4f m2;

    m1[0][2] = 4.3;
    m2[0][2] = 4.3;

    REQUIRE(m1 == m2);
}