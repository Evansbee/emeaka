#include "emeaka_matrix.h"
#include <iostream>

template <>
float Matrix<2>::Determinant() const
{
    return d[0][0] * d[1][1] - d[0][1] * d[1][0];
}