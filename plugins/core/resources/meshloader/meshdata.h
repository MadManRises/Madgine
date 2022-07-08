#pragma once

#include "Generic/bytebuffer.h"
#include "Meta/math/boundingbox.h"
#include "render/attributedescriptor.h"
#include "render/vertex.h"

namespace Engine {
namespace Render {

    struct MeshData {

        struct Material {
            std::string mName;
            std::string mDiffuseName;
            std::string mEmissiveName;
            Vector4 mDiffuseColor = Vector4::UNIT_SCALE;
        };

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
        static std::array<AttributeDescriptor, 7> generateAttributeList()
        {
            std::array<Render::AttributeDescriptor, 7> attributeList;

            if constexpr (VertexType::template holds<Render::VertexPos_4D> || VertexType::template holds<Render::VertexPos_3D>)
                attributeList[0] = { &VertexType::mPos, "POSITION", 0, type_holder<VertexType> };

            if constexpr (VertexType::template holds<Render::VertexPos2>)
                attributeList[1] = { &VertexType::mPos2, "POSITION", 1, type_holder<VertexType> };

            if constexpr (VertexType::template holds<Render::VertexColor>)
                attributeList[2] = { &VertexType::mColor, "COLOR", 0, type_holder<VertexType> };

            if constexpr (VertexType::template holds<Render::VertexNormal>)
                attributeList[3] = { &VertexType::mNormal, "NORMAL", 0, type_holder<VertexType> };

            if constexpr (VertexType::template holds<Render::VertexUV>)
                attributeList[4] = { &VertexType::mUV, "TEXCOORD", 0, type_holder<VertexType> };

            if constexpr (VertexType::template holds<Render::VertexBoneMappings>) {
                attributeList[5] = { &VertexType::mBoneIndices, "BONEINDICES", 0, type_holder<VertexType> };
                attributeList[6] = { &VertexType::mBoneWeights, "WEIGHTS", 0, type_holder<VertexType> };
            }
            return attributeList;
        }

        MeshData() = default;

        template <typename VertexType>
        MeshData(size_t groupSize, std::vector<VertexType> vertices, std::vector<uint32_t> indices = {}, std::vector<Material> materials = {})
            : mAttributeList(generateAttributeList<VertexType>)
            , mAABB(calculateAABB(vertices))
            , mGroupSize(groupSize)
            , mVertices(std::move(vertices))
            , mVertexSize(sizeof(VertexType))
            , mIndices(std::move(indices))
            , mMaterials(std::move(materials))
        {
        }



        std::array<Render::AttributeDescriptor, 7> (*mAttributeList)();
        AABB mAABB;
        size_t mGroupSize;
        ByteBuffer mVertices;
        size_t mVertexSize;
        std::vector<uint32_t> mIndices;
        std::vector<Material> mMaterials;
    };

}
}