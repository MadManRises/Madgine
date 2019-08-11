#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::ResourceLoader<OpenGLMeshLoader, OpenGLMeshData, Resources::ThreadLocalResource> {
        OpenGLMeshLoader();

        std::shared_ptr<OpenGLMeshData> loadImpl(ResourceType *res) override;

        std::shared_ptr<OpenGLMeshData> generate(const std::vector<Vertex> &vertices);

        void update(const std::shared_ptr<OpenGLMeshData> &data, const std::vector<Vertex> &vertices);
    };

}
}