#pragma once

#include <emeaka_vector.h>
#include <emeaka_color.h>

enum RenderType : class {
    FilledCircle,
    StrokedCircle,
    FilledRect,
    StrokedRect,
    FilledTriangle,
    StrokedTriangle,
    Bitmap,
    Clear,
    Text
};

struct RenderEntry_FilledCircle
{
    vec2f p;
    
}
struct RenderEntry
{

}