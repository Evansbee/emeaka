#pragma once

#include <cmath>
//#include <intrin.h>


inline float Floor(float in)
{
   return floorf(in);
}

inline float Ceil(float in)
{
   return ceilf(in);
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
   return fabsf(in);
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