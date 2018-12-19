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
   return Floor(in);
}

inline float Abs(float in)
{
   if(in < 0.0f)
   {
      return in * -1.f;
   }
   return in;
}

inline float Sin(float angle)
{
   return 0.0f;
}

inline float Cos(float angle)
{
   return 0.0f;
}

inline float ATan2(float x, float y)
{
   return 0.0f;
}