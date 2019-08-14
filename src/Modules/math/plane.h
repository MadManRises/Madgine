#pragma once

#include "vector3.h"

namespace Engine {

struct Plane {

    Plane(const Vector3 &point, const Vector3 &normal)
        : mNormal(normal)
        , mDistance(mNormal.dotProduct(point))
    {
    }

    NormalizedVector3 mNormal;
    float mDistance;
};
}