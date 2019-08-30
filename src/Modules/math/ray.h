#pragma once

#include "vector3.h"

namespace Engine {

struct Ray {
    Vector3 mPoint = Vector3::ZERO;
    NormalizedVector3 mDir = Vector3 { Vector3::ZERO };
};
}