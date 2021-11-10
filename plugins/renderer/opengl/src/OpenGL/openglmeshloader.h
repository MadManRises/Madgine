#pragma once

#include "Madgine/resources/resourceloader.h"

#include "gpumeshloader.h"

#include "openglmeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshLoader : Resources::VirtualResourceLoaderImpl<OpenGLMeshLoader, OpenGLMeshData, GPUMeshLoader> {
        OpenGLMeshLoader();

        void generateImpl(OpenGLMeshData &data, const MeshData &mesh);
        virtual bool generate(GPUMeshData &data, const MeshData &mesh) override;
        virtual bool generate(GPUMeshData &data, MeshData &&mesh) override;

        void updateImpl(OpenGLMeshData &data, const MeshData &mesh);
        virtual void update(GPUMeshData &data, const MeshData &mesh) override;
        virtual void update(GPUMeshData &data, MeshData &&mesh) override;

        virtual void reset(GPUMeshData &data) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

RegisterType(Engine::Render::OpenGLMeshLoader);