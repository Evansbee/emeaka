#pragma once

#include <cmath>
#include <numeric>
//#include <intrin.h>



inline float Sqrt(float in)
{
   return sqrtf(in);
}

inline float Floor(float in)
{
   return (in>0)?(float(int(in))):(float(int(in-1)));
}

inline float Ceil(float in)
{
   return Floor(in)+1.0f;
}

inline float Fmod(float in1, float in2)
{
   return fmodf(in1, in2);
}

inline float Round(float in)
{
   return Floor(in+0.5f);
}

inline float Abs(float in)
{
   return (in>0)?(in):(-1.f * in);
}

inline float Sin(float angle)
{
   return sinf(angle);
}

inline float Cos(float angle)
{
   return cosf(angle);
}

inline float ATan2(float x, float y)
{
   return atan2f(x,y);
}

inline bool Equal(float a, float b, float epsilons = 100)
{
   return Abs(a-b) < std::numeric_limits<float>::epsilon() * epsilons;
}