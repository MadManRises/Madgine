#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/render/attributedescriptor.h"
#include "Modules/render/vertex.h"

#include "Modules/math/boundingbox.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Resources {

    struct MeshData;

    struct MADGINE_MESHLOADER_EXPORT MeshLoader : VirtualResourceLoaderBase<MeshLoader, MeshData, std::vector<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = VirtualResourceLoaderBase<MeshLoader, MeshData, std::vector<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

            template <typename VertexType>
            void update(size_t groupSize, VertexType *vertices, size_t vertexCount, unsigned short *indices = nullptr, size_t indexCount = 0, MeshLoader *loader = nullptr)
            {
                if (!loader)
                    loader = &MeshLoader::getSingleton();
                loader->update(getData(*this, loader), groupSize, vertices, vertexCount, indices, indexCount);
            }
        };

        MeshLoader();

        bool loadImpl(MeshData &data, ResourceType *res);
        void unloadImpl(MeshData &data, ResourceType *res);

        template <typename VertexType>
        static AABB calculateAABB(VertexType *vertices, size_t vertexCount)
        {
            Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

            for (size_t i = 0; i < vertexCount; ++i) {
                VertexType &v = vertices[i];
                Vector3 pos;
                if constexpr (VertexType::template holds<Render::VertexPos_3D>)
                    pos = v.mPos;
                else
                    pos = v.mPos.xyz();
                minP = min(pos, minP);
                maxP = max(pos, maxP);
            }
            return { minP,
                maxP };
        }

        template <typename VertexType>
        bool generate(MeshData &mesh, size_t groupSize, VertexType *vertices, size_t vertexCount, unsigned short *indices = nullptr, size_t indexCount = 0, const Filesystem::Path &texturePath = {})
        {
            std::vector<std::optional<Render::AttributeDescriptor>> attributeList;

            if constexpr (VertexType::template holds<Render::VertexPos_4D> || VertexType::template holds<Render::VertexPos_3D>)
                attributeList.emplace_back(std::in_place, &VertexType::mPos, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexPos2>)
                attributeList.emplace_back(std::in_place, &VertexType::mPos2, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexColor>)
                attributeList.emplace_back(std::in_place, &VertexType::mColor, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexNormal>)
                attributeList.emplace_back(std::in_place, &VertexType::mNormal, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            if constexpr (VertexType::template holds<Render::VertexUV>)
                attributeList.emplace_back(std::in_place, &VertexType::mUV, type_holder<VertexType>);
            else
                attributeList.emplace_back();

            return generateImpl(mesh, attributeList, calculateAABB(vertices, vertexCount), groupSize, vertices, vertexCount, sizeof(VertexType), indices, indexCount, texturePath);
        }

        virtual bool generateImpl(MeshData &mesh, const std::vector<std::optional<Render::AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices = nullptr, size_t indexCount = 0, const Filesystem::Path &texturePath = {}) = 0;

        template <typename VertexType>
        void update(MeshData &data, size_t groupSize, VertexType *vertices, size_t vertexCount, unsigned short *indices = nullptr, size_t indexCount = 0)
        {
            updateImpl(data, calculateAABB(vertices, vertexCount), groupSize, vertices, vertexCount, sizeof(VertexType), indices, indexCount);
        }

        virtual void updateImpl(MeshData &data, const AABB &bb, size_t groupSize, const void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices = nullptr, size_t indexCount = 0) = 0;

        virtual void resetImpl(MeshData &mesh) = 0;
    };

}
}