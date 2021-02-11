#pragma once

#include "Meta/math/boundingbox.h"

namespace Engine {
namespace Render {

	struct GPUMeshData {
        TextureHandle mTextureHandle = 0;
        AABB mAABB;
	};

}
}