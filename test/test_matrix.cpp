#include "catch.hpp"
#include "emeaka_matrix.h"
#include "emeaka_tuple.h"
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

    m1[1][2] = 5.1;
    REQUIRE(m1!=m2);
}

TEST_CASE("Matrix Can Be Initialized With Initializer List","[matrix]")
{
    Matrix4f m1({1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16});
    REQUIRE(m1[0][0] == 1);
    REQUIRE(m1[3][3] == 16);
    REQUIRE(m1[0][1] == 2);
    REQUIRE(m1[1][0] == 5);
    std::cout<<m1<<std::endl;
}

TEST_CASE("Matrix Multiplication works","[matrix]")
{
    Matrix4f a({1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2});
    Matrix4f b({-2,1,2,3,3,2,1,-1,4,3,6,5,1,2,7,8});
    Matrix4f identity;
    Matrix4f axb({20,22,50,48,44,54,114,108,40,58,110,102,16,26,46,42});
    REQUIRE(a*b == axb);
    REQUIRE(axb * identity == axb);
}

TEST_CASE("Matrix Tuple mutliplication works","[matrix][tuple]")
{
    Matrix4f A({1,2,3,4,2,4,4,2,8,6,4,1,0,0,0,1});
    Tuple b(1,2,3,1);
    Matrix4f identity;
    Tuple c(18,24,33,1);
    REQUIRE(A*b == c);
    REQUIRE(identity * c == c);
}

TEST_CASE("Matrix transposition","[matrix")
{
    Matrix4f A({0,9,3,0,9,8,0,8,1,8,5,3,0,0,5,8});
    Matrix4f B({0,9,1,0,9,8,8,0,3,0,5,5,0,8,3,8});
    REQUIRE(A.Transposed() == B);
    A.Transpose();
    REQUIRE(A==B);
}

TEST_CASE("Can calculate the determinant","[matrix]")
{
    Matrix2f A({1,5,-3,2});
    REQUIRE(A.Determinant() == 17);
}

TEST_CASE("Submatrix Works","[matrix]")
{
    Matrix3f x({1,2,3,  4,5,6,   7,8,9});
    Matrix2f y = x.SubMatrix(0,0);
    Matrix2f result({5,6,8,9});
    REQUIRE(y==result);


    Matrix4f A({-6,1,1,6, -8,5,8,6,-1,0,8,2,-7,1,-1,1});
    Matrix3f S(A.SubMatrix(2,1));
    Matrix3f R({-6,1,6,-8,8,6,-7,-1,1});
    REQUIRE(S==R);
}

TEST_CASE("Calculate a minor of a 3x3","[matrix]")
{
    Matrix3f A({3,5,0,2,-1,-7,6,-1,5});
    Matrix2f B(A.SubMatrix(1,0));
    REQUIRE(B.Determinant() == 25);
    REQUIRE(A.Minor(1,0) == 25);
}

TEST_CASE("Cofactors of 3x3","[matrix]")
{
    Matrix3f A({3,5,0,2,-1,-7,6,-1,5});
    REQUIRE(A.Minor(0,0)==-12);
    REQUIRE(A.Cofactor(0,0)==-12);
    REQUIRE(A.Minor(1,0)==25);
    REQUIRE(A.Cofactor(1,0)==-25);
}