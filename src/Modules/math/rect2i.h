#pragma once

#include "vector2i.h"

namespace Engine {

struct Rect2i {
    Vector2i mTopLeft, mSize;

    Rect2i &operator+=(const Vector2i &other)
    {
        mTopLeft += other;
        return *this;
    }
};

}