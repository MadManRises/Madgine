#pragma once

#include "Modules/math/vector4.h"

namespace Engine {
namespace Render {

	struct Vertex {
        Vector3 mPos;
        Vector4 mColor;
        Vector3 mNormal;
    };

}
}