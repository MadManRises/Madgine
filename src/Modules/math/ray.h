#pragma once

#include "vector3.h"

namespace Engine {

struct Ray {

    Ray(const Vector3 &point, const Vector3 &dir)
        : mPoint(point)
        , mDir(dir)
    {
    }

    Vector3 mPoint;
    Vector3 mDir;
};
}