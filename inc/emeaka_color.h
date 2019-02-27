#pragma once

#include <cstdint>
#include "emeaka_intrinsics.h"

#define RGB_TO_UINT32(r, g, b) ((uint32_t)(((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b)))
#define RGBA_TO_UINT32(r, g, b, a) ((uint32_t)(((uint8_t)(a) <<24 ) | ((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b)))

struct Color
{
    Color(float _r, float _g, float _b, float _a):r(_r),g(_g),b(_b),a(_a){}
    Color(float _r, float _g, float _b):r(_r),g(_g),b(_b),a(1.0f){}
    Color():r(0.f),g(0.f),b(0.f),a(1.f){}

    float r;
    float g;
    float b;
    float a;

    uint32_t ToColor32() const {
        uint8_t _r = Round(r*255.f);
        uint8_t _g = Round(g*255.f);
        uint8_t _b = Round(b*255.f);
        uint8_t _a = Round(a*255.f);
        return RGBA_TO_UINT32(_r,_g,_b,_a);
    }
};


