#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshloader.h"

#include "openglmeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::VirtualResourceLoaderImpl<OpenGLMeshLoader, OpenGLMeshData, GPUMeshLoader> {
        OpenGLMeshLoader();

        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;

        virtual void update(GPUMeshData &data, const MeshData &mesh) override;

        virtual void reset(GPUMeshData &data) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

REGISTER_TYPE(Engine::Render::OpenGLMeshLoader)