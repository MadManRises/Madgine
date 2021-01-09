#pragma once

#include "Modules/math/boundingbox.h"
#include "render/attributedescriptor.h"
#include "Modules/generic/bytebuffer.h"
#include "Interfaces/filesystem/path.h"
#include "render/vertex.h"

namespace Engine {
namespace Render {

    struct MeshData : ScopeBase {

        template <typename VertexType>
        static AABB calculateAABB(const std::vector<VertexType> &vertices)
        {
            Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

            for (const VertexType &v : vertices) {
                const Vector3 &pos = v.mPos.xyz();
                minP = min(pos, minP);
                maxP = max(pos, maxP);
            }
            return { minP,
                maxP };
        }

        template <typename VertexType>
        static std::vector<std::optional<AttributeDescriptor>> generateAttributeList()
        {
            std::vector<std::optional<Render::AttributeDescriptor>> attributeList;

            if constexpr (VertexType::template holds<Render::VertexPos_4D> || VertexType::template holds<Render::VertexPos_3D>)
                attributeList.emplace_back(std::in_place, &VertexType::mPos, "POSITION", 0, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexPos2>)
                attributeList.emplace_back(std::in_place, &VertexType::mPos2, "POSITION", 1, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexColor>)
                attributeList.emplace_back(std::in_place, &VertexType::mColor, "COLOR", 0, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexNormal>)
                attributeList.emplace_back(std::in_place, &VertexType::mNormal, "NORMAL", 0, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexUV>)
                attributeList.emplace_back(std::in_place, &VertexType::mUV, "TEXCOORD", 0, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexBoneMappings>) {
                attributeList.emplace_back(std::in_place, &VertexType::mBoneIndices, "BONEINDICES", 0, type_holder<VertexType>);
                attributeList.emplace_back(std::in_place, &VertexType::mBoneWeights, "WEIGHTS", 0, type_holder<VertexType>);
            } else {
                attributeList.emplace_back();
                attributeList.emplace_back();
            }
            return attributeList;
        }

        MeshData() = default;

        template <typename VertexType>
        MeshData(size_t groupSize, std::vector<VertexType> vertices, std::vector<unsigned short> indices = {}, Filesystem::Path texturePath = {})
            : mAttributeList(generateAttributeList<VertexType>())
            , mAABB(calculateAABB(vertices))
            , mGroupSize(groupSize)
            , mVertices(std::move(vertices))
            , mVertexSize(sizeof(VertexType))
            , mIndices(std::move(indices))
            , mTexturePath(std::move(texturePath))
        {
        }

        std::vector<std::optional<Render::AttributeDescriptor>> mAttributeList;
        AABB mAABB;
        size_t mGroupSize;
        ByteBuffer mVertices;
        size_t mVertexSize;
        std::vector<unsigned short> mIndices;
        Filesystem::Path mTexturePath;
    };

}
}