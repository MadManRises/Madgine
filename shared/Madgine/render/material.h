#pragma once

#include "Meta/math/vector4.h"
#include "texture.h"

namespace Engine {
namespace Render {

struct Material {
        TextureHandle mDiffuseTexture;
        TextureHandle mEmissiveTexture;
        Vector4 mDiffuseColor = Vector4::UNIT_SCALE;
    };

}
}