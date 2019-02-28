#pragma once

#include <cstdlib>
#include <iostream>

//always 4x4
template <size_t N, size_t M = N>
struct Matrix
{
    Matrix()
    {
        for(auto y = 0; y < M; ++y)
        {
            for(auto x = 0; x < N; ++x)
            {
                d[x][y] = 0;
            }
        }
    }
    struct MatrixRow
    {
        float& operator [](size_t col)
        {
            return row[col];
        }
        float *row;
    };

    MatrixRow& operator [](size_t row)
    {
        mr.row = (float *)&d[row];
        return mr;
    }
    float d[N][M];

    template <size_t X, size_t Y>
    friend std::ostream& operator <<(std::ostream& os, Matrix<X,Y>& m);
    private:
    MatrixRow mr;
};

template <size_t N, size_t M = N>
std::ostream& operator <<(std::ostream& os, Matrix<N,M>& m)
{
    os<<"[";
    for(size_t row = 0; row < N; ++row)
    {
        os<<"[";
        for(size_t col = 0; col < M; ++col)
        {
            os<<m[row][col];
            if(col < M-1)
            {
                os<<", ";
            }
        }
        os<<"]";
        if(row < N - 1)
        {
            os<<",";
        }
    }
    os<<"]";
    return os;
}