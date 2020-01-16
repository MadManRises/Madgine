#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/render/attributedescriptor.h"
#include "Modules/render/vertex.h"

#include "Modules/math/boundingbox.h"

#include "Modules/threading/workgroupstorage.h"

#include "Modules/render/bytebuffer.h"

#include "meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_MESHLOADER_EXPORT MeshLoader : Resources::VirtualResourceLoaderBase<MeshLoader, MeshData, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = VirtualResourceLoaderBase<MeshLoader, MeshData, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

            template <typename VertexType>
            void update(size_t groupSize, std::vector<VertexType> vertices, std::vector<unsigned short> indices = {}, MeshLoader *loader = nullptr)
            {
                if (!loader)
                    loader = &MeshLoader::getSingleton();
                loader->update(getData(*this, loader), groupSize, std::move(vertices), std::move(indices));
            }
        };

        MeshLoader();

        bool loadImpl(MeshData &data, ResourceType *res);
        void unloadImpl(MeshData &data, ResourceType *res);

        template <typename VertexType>
        static AABB calculateAABB(const std::vector<VertexType> &vertices)
        {
            Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

            for (const VertexType &v : vertices) {                
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
        bool generate(MeshData &mesh, size_t groupSize, std::vector<VertexType> vertices, std::vector<unsigned short> indices = {}, const Filesystem::Path &texturePath = {})
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

			AABB aabb = calculateAABB(vertices);
            return generateImpl(mesh, attributeList, std::move(aabb), groupSize, std::move(vertices), sizeof(VertexType), std::move(indices), texturePath);
        }

        virtual bool generateImpl(MeshData &mesh, const std::vector<std::optional<Render::AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, Render::ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices = {}, const Filesystem::Path &texturePath = {}) = 0;

        template <typename VertexType>
        void update(MeshData &mesh, size_t groupSize, std::vector<VertexType> vertices, std::vector<unsigned short> indices = {})
        {
            AABB aabb = calculateAABB(vertices);
            updateImpl(mesh, std::move(aabb), groupSize, std::move(vertices), sizeof(VertexType), std::move(indices));
        }

        virtual void updateImpl(MeshData &mesh, const AABB &bb, size_t groupSize, Render::ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices = {}) = 0;

        virtual void resetImpl(MeshData &mesh) = 0;
    };

}
}