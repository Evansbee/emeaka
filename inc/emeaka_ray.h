#pragma once

#include "emeaka_tuple.h"
#include "emeaka_matrix.h"

struct Ray
{
    Ray(const Tuple& o, const Tuple& d):Origin(o),Direction(d){}
    Ray():Origin(Point(0,0,0)),Direction(Vector(0,0,0)){}
    Ray(const Ray& Other)
    {
        Origin = Other.Origin;
        Direction = Other.Direction;
    }
    Tuple Origin;
    Tuple Direction;
    Tuple Position(float distance) {return Origin + (Direction * t);}
}