#pragma once

#include <cstdlib>
#include <cstdio>
#include <initializer_list>

#include "emeaka_tuple.h"

template <size_t N>
struct Matrix
{
    Matrix()
    {
        memset(*d, 0, N * N * sizeof(float));
        for (size_t i = 0; i < N; ++i)
        {
            d[i][i] = 1.f;
        }
    }

    Matrix(std::initializer_list<float> v)
    {
        if (v.size() != N * N)
        {
            for (size_t i = 0; i < N; ++i)
            {
                d[i][i] = 1.f;
            }
        }
        else
        {
            size_t i = 0;
            for (auto &item : v)
            {
                ((float *)d)[i++] = item;
            }
        }
    }

    Matrix &operator=(const Matrix &other)
    {
        for (auto y = 0; y < N; ++y)
        {
            for (auto x = 0; x < N; ++x)
            {
                d[x][y] = other.d[x][y];
            }
        }
    }

    bool operator==(const Matrix &other) const
    {
        for (auto y = 0; y < N; ++y)
        {
            for (auto x = 0; x < N; ++x)
            {
                if (d[x][y] != other.d[x][y])
                {
                    return false;
                }
            }
        }
        return true;
    }

    Tuple operator*(const Tuple &other) const
    {
        static_assert(N == 4,"Matrix * Tuple multiplication only defined for 4x4 matrixes");
        Tuple result;
        for (size_t row = 0; row < N; ++row)
        {

            float value = 0;
            for (size_t i = 0; i < N; ++i)
            {
                value += d[row][i] * other.d[i];
            }
            result.d[row] = value;
        }
        return result;
    }

    Matrix operator*(const Matrix &other) const
    {
        Matrix result;
        for (size_t row = 0; row < N; ++row)
        {
            for (size_t col = 0; col < N; ++col)
            {
                float value = 0;
                for (size_t i = 0; i < N; ++i)
                {
                    value += d[row][i] * other[i][col];
                }
                result[row][col] = value;
            }
        }
        return result;
    }

    void Transpose()
    {
        for (size_t row = 0; row < (N/2); ++row)
        {
            for (size_t col = 0; col < (N/2); ++col)
            {
                float temp = d[row][col];
                d[row][col] = d[row][col];
                d[col][row] = temp;
            }
        }
    }

    Matrix Transposed(){
        Matrix nm(*this);
        nm.Transpose();
        return nm;
    }

    float Determinant() const
    {
        static_assert(N==2, "Only supporting 2x2 determinant");
        return d[0][0] * d[1][1] - d[0][1] * d[1][0];
    }

    Matrix<N-1> SubMatrix(size_t row, size_t col) const
    {
        Matrix<N-1> s;
        
        for (size_t r = 0; r < (N); ++r)
        {
            if(r != row)
            {
                for (size_t c = 0; c < (N); ++c)
                {
                    if(c != col)
                    {   
                        size_t wr = (r>row)?(r-1):r; 
                        size_t wc = (c>col)?(c-1):c;
                        s[wr][wc] = d[r][c];
                    }
                }
            }
        }
        return s;
    }

    bool operator!=(const Matrix &other) const
    {
        return !(*this == other);
    }

    const float *operator[](size_t row) const
    {
        return d[row];
    }

    float *operator[](size_t row)
    {
        return d[row];
    }
    float d[N][N];

    //all templates are my friend -- which seems to fix it....
    template <size_t X>
    friend std::ostream &operator<<(std::ostream &os, const Matrix<X> &m);
};

typedef Matrix<4> Matrix4f;
typedef Matrix<3> Matrix3f;
typedef Matrix<2> Matrix2f;

template <size_t X>
std::ostream &operator<<(std::ostream &os, const Matrix<X> &m)
{
    os << "[";
    for (size_t row = 0; row < X; ++row)
    {
        os << "[";
        for (size_t col = 0; col < X; ++col)
        {
            os << m[row][col];
            if (col < X - 1)
            {
                os << ", ";
            }
        }
        os << "]";
        if (row < X - 1)
        {
            os << ",";
        }
    }
    os << "]";
    return os;
}
