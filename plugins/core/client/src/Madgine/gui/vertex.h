#pragma once

#include "Modules/math/vector4.h"

namespace Engine {
namespace GUI {

    struct Vertex {

        Vector3 mPos;
        Vector4 mColor;
        Vector2 mUV = {};
    };

}
}