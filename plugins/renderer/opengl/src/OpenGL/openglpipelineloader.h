#pragma once

#include "Madgine/resources/resourceloader.h"

#include "pipelineloader.h"

#include "util/openglpipeline.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLPipelineLoader : Resources::VirtualResourceLoaderImpl<OpenGLPipelineLoader, OpenGLPipeline, PipelineLoader> {
        OpenGLPipelineLoader();

        bool loadImpl(OpenGLPipeline &pipeline, ResourceDataInfo &info);
        void unloadImpl(OpenGLPipeline &pipeline);
        Threading::Task<bool> create(Instance &instance, PipelineConfiguration config, bool dynamic) override;
        //bool create(Program &program, const std::string &name, const CodeGen::ShaderFile &file) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

REGISTER_TYPE(Engine::Render::OpenGLPipelineLoader)