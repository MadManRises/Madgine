#pragma once

#include "vector2.h"

namespace Engine {

struct Rect2 {
    Vector2 mTopLeft, mSize;

    Rect2 &operator+=(const Vector2 &other)
    {
        mTopLeft += other;
        return *this;
    }

    float bottom() const {
        return mTopLeft.y + mSize.y;
    }
};

}