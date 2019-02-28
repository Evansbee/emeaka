#include "catch.hpp"
#include "emeaka_matrix.h"
#include <iostream>

TEST_CASE("Matrix wtf","[matrix]")
{
    Matrix<4> m;
    m.d[0][0] = 1.0f;
    m.d[1][3] = 2.0f;
    REQUIRE(m[0][0] == 1.0f);
    REQUIRE(m[1][3] == 2.0f);
    std::cout<<m<<std::endl;
}

TEST_CASE("Matrix can be equal","[matrix]")
{
    Matrix<3> m1;
    Matrix<3> m2;

    m1[0][2] = 4.3;
    m2[0][2] = 4.3;

    REQUIRE(m1 == m2);
    std::cout<<m1<<std::endl;
}