#include "emeaka_matrix.h"
#include "emeaka_intrinsics.h"
#include <iostream>

template <>
float Matrix<2>::Determinant() const
{
    return d[0][0] * d[1][1] - d[0][1] * d[1][0];
}

Matrix<4> Translation(float x, float y, float z)
{
    Matrix4f translationMatrix;
    translationMatrix[0][3] = x;
    translationMatrix[1][3] = y;
    translationMatrix[2][3] = z;
    return translationMatrix;
}
Matrix<4> Scaling(float s)
{
    return Scaling(s,s,s);
}

Matrix<4> Scaling(float x, float y, float z)
{
    Matrix4f scalingMatrix;
    scalingMatrix[0][0] = x;
    scalingMatrix[1][1] = y;
    scalingMatrix[2][2] = z;
    return scalingMatrix;
}

Matrix<4> RotationX(float r){
    Matrix4f rotationMatrix;
    rotationMatrix[1][1] = Cos(r);
    rotationMatrix[1][2] = -Sin(r);
    rotationMatrix[2][1] = Sin(r);
    rotationMatrix[2][2] = Cos(r);
    return rotationMatrix;
}
Matrix<4> RotationY(float r){
    Matrix4f rotationMatrix;
    rotationMatrix[0][0] = Cos(r);
    rotationMatrix[0][2] = Sin(r);
    rotationMatrix[2][0] = -Sin(r);
    rotationMatrix[2][2] = Cos(r);
    return rotationMatrix;
}
Matrix<4> RotationZ(float r){
    Matrix4f rotationMatrix;
    rotationMatrix[0][0] = Cos(r);
    rotationMatrix[0][1] = -Sin(r);
    rotationMatrix[1][0] = Sin(r);
    rotationMatrix[1][1] = Cos(r);
    return rotationMatrix;
}