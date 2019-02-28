#include "emeaka_tuple.h"

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