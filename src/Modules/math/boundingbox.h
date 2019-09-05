#pragma once

#include "vector3.h"

namespace Engine {

struct AABB {
    Vector3 mMin, mMax;

    constexpr float diameter() const
    {
        return (mMax - mMin).length();
    }

    constexpr Vector3 center() const
    {
        return 0.5f * (mMax + mMin);
    }
};

}