#pragma once

#include "Modules/resources/resourceloader.h"

#include "meshloader.h"

#include "openglmeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::VirtualResourceLoaderImpl<OpenGLMeshLoader, OpenGLMeshData, MeshLoader> {
        OpenGLMeshLoader();

        virtual bool generateImpl(MeshData &mesh, const std::vector<std::optional<AttributeDescriptor>> &attributeList, const AABB &bb, size_t groupSize, ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices = {}, const Filesystem::Path &texturePath = {}) override;

        virtual void updateImpl(MeshData &data, const AABB &bb, size_t groupSize, ByteBuffer vertices, size_t vertexSize, std::vector<unsigned short> indices = {}) override;

		virtual void resetImpl(MeshData &data) override;
    };
}
}