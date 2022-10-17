#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Madgine/pipelineloader/pipelineloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11PipelineLoader : Resources::VirtualResourceLoaderImpl<DirectX11PipelineLoader, Pipeline, PipelineLoader> {
        DirectX11PipelineLoader();

        bool loadImpl(Pipeline &pipeline, ResourceDataInfo &info);
        void unloadImpl(Pipeline &pipeline);
        Threading::Task<bool> create(Instance &instance, PipelineConfiguration config) override;
        Threading::Task<bool> create(Instance &instance, PipelineConfiguration config, CodeGen::ShaderFile file) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

REGISTER_TYPE(Engine::Render::DirectX11PipelineLoader)