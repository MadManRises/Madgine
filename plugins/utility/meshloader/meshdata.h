#pragma once

#include "Modules/math/boundingbox.h"

namespace Engine {
namespace Resources {

	struct MeshData {
        unsigned int mTextureHandle = 0;
        AABB mAABB;
	};

}
}