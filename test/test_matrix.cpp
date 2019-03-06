#include "catch.hpp"
#include "emeaka_matrix.h"
#include "emeaka_tuple.h"
#include "emeaka_intrinsics.h"
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

TEST_CASE("Matrix transposition","[matrix]")
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

TEST_CASE("Determinante of 3x3 matrix","[matrix]")
{
    Matrix3f A({1,2,6,-5,8,-4,2,6,4});
    REQUIRE(A.Cofactor(0,0)==56);
    REQUIRE(A.Cofactor(0,1)==12);
    REQUIRE(A.Cofactor(0,2)==-46);
    REQUIRE(A.Determinant()==-196);
}

TEST_CASE("Determinante of 4x4 matrix","[matrix]")
{
    Matrix4f A({-2,-8,3,5,-3,1,7,3,1,2,-9,6,-6,7,7,-9});
    REQUIRE(A.Cofactor(0,0)==690);
    REQUIRE(A.Cofactor(0,1)==447);
    REQUIRE(A.Cofactor(0,2)==210);
    REQUIRE(A.Cofactor(0,3)==51);
    REQUIRE(A.Determinant()==-4071);
}

TEST_CASE("Determine invertability","[matrix]")
{
    Matrix4f A({6,4,4,4,5,5,7,6,4,-9,3,-7,9,1,7,-6});
    REQUIRE(A.Determinant() == -2120);
    REQUIRE(A.IsInvertable());

    Matrix4f B({-4,2,-2,-3,9,6,2,6,0,-5,1,-5,0,0,0,0});
    REQUIRE(B.Determinant() == 0);
    REQUIRE_FALSE(B.IsInvertable());
}

TEST_CASE("Calculating the inverse matrix @ 4x4","[matrix]")
{
    Matrix4f A({-5,2,6,-8,1,-5,1,8,7,7,-6,-7,1,-3,7,4});
    Matrix4f B = A.Inverted();
    Matrix4f C(A);
    C.Invert();

    REQUIRE(A.Determinant()==532);
    REQUIRE(A.Cofactor(2,3)==-160);
    REQUIRE(B[3][2] == -160.f/532.f);
    REQUIRE(A.Cofactor(3,2) == 105);
    REQUIRE(B[2][3] == 105.f/532.f);
    REQUIRE(B==C);

    Matrix4f D({8,-5,9,2,7,5,6,1,-6,0,9,6,-3,0,-9,-4});
    D.Invert();
    REQUIRE(D[0][0] == Approx(-0.15385f).epsilon(.0001));
    REQUIRE(D[0][1] == Approx(-0.15385f).epsilon(.0001));
    REQUIRE(D[0][2] == Approx(-0.28205f).epsilon(.0001));
    REQUIRE(D[0][3] == Approx(-0.53846f).epsilon(.0001));
    REQUIRE(D[1][0] == Approx(-0.07692f).epsilon(.0001));
    REQUIRE(D[1][1] == Approx( 0.12308f).epsilon(.0001));
    REQUIRE(D[1][2] == Approx( 0.02564f).epsilon(.0001));
    REQUIRE(D[1][3] == Approx( 0.03077f).epsilon(.0001));
    REQUIRE(D[2][0] == Approx( 0.35897f).epsilon(.0001));
    REQUIRE(D[2][1] == Approx( 0.35897f).epsilon(.0001));
    REQUIRE(D[2][2] == Approx( 0.43590f).epsilon(.0001));
    REQUIRE(D[2][3] == Approx( 0.92308f).epsilon(.0001));
    REQUIRE(D[3][0] == Approx(-0.69231f).epsilon(.0001));
    REQUIRE(D[3][1] == Approx(-0.69231f).epsilon(.0001));
    REQUIRE(D[3][2] == Approx(-0.76923f).epsilon(.0001));
    REQUIRE(D[3][3] == Approx(-1.92308f).epsilon(.0001));
    
}

TEST_CASE("Multiplication of a trsnlation matrix","[transform]")
{
    Matrix4f transform = Translation(5,-3,2);
    Tuple p = Point(-3,4,5);
    REQUIRE((transform * p) == Point(2,1,7));
}

TEST_CASE("Multiplying by the inverse of the xform","[transform]")
{
    Matrix4f transform = Translation(5,-3,2);
    Matrix4f inv = transform.Inverted();
    Tuple p = Point(-3,4,5);
    REQUIRE((inv * p) == Point(-8,7,3));
}

TEST_CASE("Translation doesn't affect vectors","[transform]")
{
    Matrix4f transform = Translation(5,-3,2);
    Tuple v = Vector(-3,4,5);
    REQUIRE((transform * v)==v);
}

TEST_CASE("Scaling Matrix applied to a point","[transform]")
{
    Matrix4f transform = Scaling(2,3,4);
    Tuple p = Point(-4,6,8);
    REQUIRE((transform * p) == Point(-8,18,32));
}

TEST_CASE("Scaling Matrix applied to a vector","[transform]")
{
    Matrix4f transform = Scaling(2,3,4);
    Tuple v = Vector(-4,6,8);
    REQUIRE((transform * v) == Vector(-8,18,32));
}

TEST_CASE("Multiplying by the inverse of a scaling matrix","[transform]")
{
    Matrix4f t = Scaling(2,3,4);
    Matrix4f i = t.Inverted();
    Tuple v = Vector(-4,6,8);
    REQUIRE((i * v) == Vector(-2,2,2));
}

TEST_CASE("Reflection is scaling by a negative value","[transform]")
{
    Matrix4f t = Scaling(-1,1,1);
    Tuple p = Point(2,3,4);
    REQUIRE((t * p) == Point(-2,3,4));
}

TEST_CASE("Rotate point around x axis","[transform]")
{
    Tuple p = Point(0,1,0);
    Matrix4f half_quarter = RotationX( Pi() / 4.f);
    Matrix4f full_quarter = RotationX(Pi() / 2.f);
    REQUIRE((half_quarter * p) == Point(0, Sqrt(2)/2.f, Sqrt(2)/2.f));
    CHECK((full_quarter * p) == Point(0, 0, 1));
}

//need more rotation matrices