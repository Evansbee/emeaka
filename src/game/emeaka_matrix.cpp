#include "emeaka_matrix.h"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const Matrix4f &m)
{
    os << "[";
    for (size_t row = 0; row < 4; ++row)
    {
        os << "[";
        for (size_t col = 0; col < 4; ++col)
        {
            os << m[row][col];
            if (col < 4 - 1)
            {
                os << ", ";
            }
        }
        os << "]";
        if (row < 4 - 1)
        {
            os << ",";
        }
    }
    os << "]";
    return os;
}