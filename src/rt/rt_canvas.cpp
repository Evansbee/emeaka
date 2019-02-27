#include <cstdio>
#include <cstdint>
#include <cstdlib>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "rt_canvas.h"

Canvas::Canvas(int width, int height) : Width(width), Height(height)
{
    Buffer = (Color *)malloc(sizeof(Color) * Width * Height);
    memset((void *)Buffer,0,sizeof(Color) * Width * Height);
}

void Canvas::Write(int x, int y, const Color &c)
{
    Buffer[y * Width + x] = c;
}   

Color Canvas::Read(int x, int y) const
{
    return Buffer[y * Width + x];
}

void Canvas::SaveHDR(const char *fileName)
{
    stbi_write_hdr(fileName, Width, Height, sizeof(Color)/sizeof(float), (float *)Buffer);
}

Canvas::~Canvas()
{
    free(Buffer);
    Buffer = nullptr;
}