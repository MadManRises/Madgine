#pragma once

#include "Meta/math/boundingbox.h"
#include "textureloader.h"

namespace Engine {
namespace Render {

    struct GPUMeshData {
        struct Material {
            std::string mName;
            TextureLoader::HandleType mDiffuseTexture;
            TextureLoader::HandleType mEmissiveTexture;
            TextureHandle mDiffuseHandle = 0;
            TextureHandle mEmissiveHandle = 0;
            Vector4 mDiffuseColor = Vector4::UNIT_SCALE;
        };
        std::vector<Material> mMaterials;
        AABB mAABB;
    };

}
}