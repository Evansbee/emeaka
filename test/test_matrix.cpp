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

TEST_CASE("Matrix constructs as identity","[matrix]")
{
    Matrix2f m2;
    Matrix3f m3;
    Matrix4f m4;

    for(auto i = 0; i < 2; ++i)
    {
        for(auto j = 0; j < 2; ++j)
        {
            if(i == j)
            {
                if(m2[i][j] != 1.f)
                {
                    FAIL("Identity Matrix");
                }
            }
            else
            {
                if(m2[i][j] != 0.f)
                {
                    FAIL("Identity Matrix");
                }
            }   
        }
    }
    for(auto i = 0; i < 3; ++i)
    {
        for(auto j = 0; j < 3; ++j)
        {
            if(i == j)
            {
                if(m3[i][j] != 1.f)
                {
                    FAIL("Identity Matrix");
                }
            }
            else
            {
                if(m3[i][j] != 0.f)
                {
                    FAIL("Identity Matrix");
                }
            }   
        }
    }
    for(auto i = 0; i < 4; ++i)
    {
        for(auto j = 0; j < 4; ++j)
        {
            if(i == j)
            {
                if(m4[i][j] != 1.f)
                {
                    FAIL("Identity Matrix");
                }
            }
            else
            {
                if(m4[i][j] != 0.f)
                {
                    FAIL("Identity Matrix");
                }
            }   
        }
    }
    SUCCEED("New Matrix Constructed As Identity (2, 3 & 4)");
}

TEST_CASE("Matrix can be equal/notEqual","[matrix]")
{
    Matrix4f m1;
    Matrix4f m2;

    m1[0][2] = 4.3;
    m2[0][2] = 4.3;

    REQUIRE(m1 == m2);
}

