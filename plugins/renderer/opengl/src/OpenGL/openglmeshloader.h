#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::ResourceLoader<OpenGLMeshLoader, OpenGLMeshData, Resources::ThreadLocalResource> {
        OpenGLMeshLoader();

        std::shared_ptr<OpenGLMeshData> loadImpl(ResourceType *res) override;

        static OpenGLMeshData generate(size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0);
        static OpenGLMeshData generate(size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0);

        static void update(OpenGLMeshData &data, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0);
        static void update(OpenGLMeshData &data, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices = nullptr, size_t indexCount = 0);
    };

}
}