#pragma once

#include "Modules/resources/resourceloader.h"

#include "meshloader.h"


namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::VirtualResourceLoaderImpl<OpenGLMeshLoader, Resources::MeshLoader> {
        OpenGLMeshLoader();

        virtual std::shared_ptr<Resources::MeshData> generateImpl(const std::vector<std::optional<AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices = nullptr, size_t indexCount = 0, const Filesystem::Path &texturePath = {}) override;

        virtual void updateImpl(Resources::MeshData &data, const AABB &bb, size_t groupSize, const void *vertices, size_t vertexCount, size_t vertexSize, unsigned short *indices = nullptr, size_t indexCount = 0) override;
    };
}
}