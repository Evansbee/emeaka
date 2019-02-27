#pragma once
#include "emeaka_intrinsics.h"

#include <iostream>

struct Tuple
{
    Tuple(float _x, float _y, float _z, float _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
        {}
    float x, y, z, w;

    bool IsPoint() const { return w == 1.0f; }
    bool IsVector() const { return w == 0.0f; }
    bool operator ==(const Tuple& other) const {
        return other.x == x && other.y == y && other.z == z && other.w == w;
        return Equal(other.x, x) && Equal(other.y,y) && Equal(other.z,z) && Equal(other.w,w);
    }

    Tuple& operator =(const Tuple& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    Tuple operator +(const Tuple& b) const
    {
        return Tuple(x+b.x, y+b.y, z+b.z, w+b.w);
    }
    Tuple operator -(const Tuple& b) const 
    {
        return Tuple(x-b.x, y-b.y, z-b.z, w-b.w);
    }
    Tuple operator *(float b) const
    {
        return Tuple(x*b, y*b, z*b, w*b);
    }
    Tuple operator /(float b) const 
    {
        return Tuple(x/b, y/b, z/b, w/b);
    }
    Tuple operator -() const
    {
        return Tuple(-x,-y,-z,-w);
    }

    float LengthSquared() const 
    {
        return x*x + y*y + z*z + w*w;
    }
    float Length() const
    {
        return Sqrt(x*x + y*y + z*z + w*w);
    }

    void Normalize()
    {
        float len = Length();
        x/=len;
        y/=len;
        z/=len;
        w/=len;
    }

    Tuple Normalized() const
    {
        Tuple t = *this;
        t.Normalize();
        return t;
    }

    friend std::ostream& operator<<(std::ostream& os, const Tuple& t);
};

std::ostream& operator<<(std::ostream& os, const Tuple& t)
{
    os << "("<<t.x<<","<<t.y<<","<<t.z<<","<<t.w<<")";
    return os;
}

Tuple Point(float x, float y, float z)
{
    return Tuple(x,y,z,1.f);
}

Tuple Vector(float x, float y, float z)
{
    return Tuple(x,y,z,0.f);
}

float Dot(const Tuple& a, const Tuple& b)
{
    return (a.x * b.x) + 
           (a.y * b.y) + 
           (a.z * b.z) + 
           (a.w * b.w);
}

Tuple Cross(const Tuple& a, const Tuple& b)
{
    return Vector(a.y * b.z - a.z * b.y,
                  a.z * b.x - a.x * b.z,
                  a.x * b.y - a.y * b.x);
}