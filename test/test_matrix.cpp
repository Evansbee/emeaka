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
