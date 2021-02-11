#pragma once

#include "vector3.h"
#include "matrix4.h"

namespace Engine {

struct Sphere {


	Vector3 mCenter;
    float mRadius;
};

inline Sphere operator*(const Matrix4& m, const Sphere& s) {
    return {
        (m * Vector4 { s.mCenter, 1.0f }).xyz(),
        s.mRadius
    }; //TODO scaling
}

}