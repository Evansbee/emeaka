#pragma once

#include <iostream>

struct Color
{
    Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
    Color() : Color(0,0,0) {}

    float r;
    float g;
    float b;

    bool operator ==(const Color& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    bool operator !=(const Color& other) const
    {
        return !(*this==other);
    }
    Color& operator =(const Color& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }

    Color operator +(const Color& other) const
    {
        return Color(r+other.r, g+other.g, b+other.b);
    }

    Color operator -(const Color& other) const
    {
        return Color(r-other.r, g-other.g, b-other.b);
    }
    Color operator *(const Color& other) const
    {
        //hadddamarrrdd?
        return Color(r*other.r, g*other.g, b*other.b);
    }
    Color operator *(float other) const
    {
        return Color(r*other, g*other, b*other);
    }
    friend std::ostream& operator<<(std::ostream& os, const Color& c);

};

//template <typename T>
static Color operator *(float other, const Color& c)
{
    float f = float(other);
    return c * other;
}


struct Canvas
{
    Canvas() = delete;
    Canvas(int width, int height);
    ~Canvas();
    Color *Buffer;
    void Write(int x, int y, const Color& c);
    Color Read(int x, int y) const;
    void SaveHDR(const char* fileName);
    int Width;
    int Height;

};