#pragma once

#include <cstdint>

template <typename T>
struct vec4_t
{
    vec4_t():x(T()),y(T()),z(T()),w(T(1)){}
    vec4_t(T _x, T _y, T _z, T _w):x(_x),y(_y),z(_z),w(_w){}
    T x,y,z,w;

    vec4_t& operator =(const vec4_t& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }
    
};

template <typename T>
struct vec3_t
{
    vec3_t():x(T()),y(T()),z(T()){}
    vec3_t(T _x, T _y, T _z):x(_x),y(_y),z(_z){}
    T x,y,z;
    vec3_t& operator =(const vec3_t& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
};

template <typename T>
struct vec2_t
{
    vec2_t():x(T()),y(T()){}
    vec2_t(T _x, T _y):x(_x),y(_y){}
    T x,y;
    vec2_t& operator =(const vec2_t& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
};

typedef vec2_t<float> vec2f;
typedef vec2_t<int64_t> vec2i;
typedef vec2_t<uint64_t> vec2u;

typedef vec3_t<float> vec3f;
typedef vec3_t<int64_t> vec3i;
typedef vec3_t<uint64_t> vec3u;

typedef vec4_t<float> vec4f;
typedef vec4_t<int64_t> vec4i;
typedef vec4_t<uint64_t> vec4u;