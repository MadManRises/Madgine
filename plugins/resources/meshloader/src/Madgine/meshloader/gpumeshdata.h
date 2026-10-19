#pragma once

#include "Meta/math/boundingbox.h"

#include "Madgine/render/resourceblock.h"
#include "Madgine/render/vertexformat.h"

namespace Engine {
namespace Render {

    struct GPUMeshData {

        // Temporary solution to handle ResourceBlocks
        struct ResourceBlockHolder {
            UniqueResourceBlock mResourceBlock;
        };

        struct Material {
            std::string mName;
            std::shared_ptr<ResourceBlockHolder> mResourceBlock;          
            Math::Vector4 mDiffuseColor = Math::Vector4::UNIT_SCALE;
            // Light emitted by the surface itself (added after lighting, e.g. lamps, displays)
            Math::Vector3 mEmissiveColor = Math::Vector3::ZERO;
        };
        std::vector<Material> mMaterials;
        Math::AABB mAABB;
        VertexFormat mFormat;
        size_t mGroupSize;
        size_t mElementCount;

        GPUPtr<Void[]> mVertices;
        GPUPtr<uint32_t[]> mIndices;
    };

}
}