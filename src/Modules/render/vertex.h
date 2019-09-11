#pragma once

#include "../math/vector4.h"

namespace Engine {
namespace Render {

	struct Vertex {
        Vector3 mPos;
        Vector4 mColor;
        Vector3 mNormal;
    };
	
	struct Vertex2 {
        Vector3 mPos;
        Vector2 mPos2;
        Vector4 mColor;
        Vector2 mUV;
	};

}
}