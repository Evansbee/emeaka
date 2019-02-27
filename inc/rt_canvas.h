#pragma once

struct Color
{
    Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
    Color() : Color(0,0,0) {}

    float r;
    float g;
    float b;
};

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