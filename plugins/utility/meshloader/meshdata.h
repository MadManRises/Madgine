#pragma once

#include "Modules/math/boundingbox.h"

namespace Engine {
namespace Render {

	struct MeshData {
        TextureHandle mTextureHandle = 0;
        AABB mAABB;
	};

}
}