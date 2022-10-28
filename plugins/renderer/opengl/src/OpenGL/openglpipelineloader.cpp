#include "opengllib.h"

#include "openglpipelineloader.h"

#include "openglshaderloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/codegen/codegen_shader.h"

#include "openglrendercontext.h"

#include "util/openglpipelineinstance.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLPipelineLoader);

METATABLE_BEGIN_BASE(Engine::Render::OpenGLPipelineLoader, Engine::Render::PipelineLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLPipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLPipelineLoader::Resource, Engine::Render::PipelineLoader::Resource)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Render::OpenGLPipelineLoader::Resource)

namespace Engine {
namespace Render {

    OpenGLPipelineLoader::OpenGLPipelineLoader()
    {
    }

    bool OpenGLPipelineLoader::loadImpl(OpenGLPipeline &pipeline, ResourceDataInfo &info)
    {
        throw 0;
    }

    void OpenGLPipelineLoader::unloadImpl(OpenGLPipeline &pipeline)
    {
        pipeline.reset();
    }

    Threading::Task<bool> OpenGLPipelineLoader::create(Instance &instance, PipelineConfiguration config)
    {

        char buffer[256];
#if WINDOWS
        sprintf_s(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#else
        sprintf(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#endif

        Handle pipeline;
        if (!co_await pipeline.create(buffer, {}, [vs { std::string { config.vs } }, ps { std::string { config.ps } }, gs { std::string { config.gs } }](OpenGLPipelineLoader *loader, OpenGLPipeline &pipeline, ResourceDataInfo &info) -> Threading::Task<bool> {
                OpenGLShaderLoader::Handle vertexShader;
                if (!co_await vertexShader.load(vs, VertexShader)) {
                    LOG_ERROR("Failed to load VS '" << vs << "'!");
                    co_return false;
                }

                OpenGLShaderLoader::Handle pixelShader;
                if (!ps.empty() && !co_await pixelShader.load(ps, PixelShader)) {
                    LOG_ERROR("Failed to load PS '" << ps << "'!");
                    co_return false;
                }

                OpenGLShaderLoader::Handle geometryShader;
                if (!gs.empty() && !co_await geometryShader.load(gs, GeometryShader)) {
                    LOG_ERROR("Failed to load GS '" << gs << "'!");
                    co_return false;
                }

                if (!pipeline.link(std::move(vertexShader), std::move(geometryShader), std::move(pixelShader))) {
                    LOG_ERROR("Failed to link Program '" << vs << "|" << gs << "|" << ps
                                                         << "'!");
                    co_return false;
                }

                co_return true;
            }))
            co_return false;

        instance = std::make_unique<OpenGLPipelineInstance>(config, std::move(pipeline));

        co_return true;
    }

    Threading::Task<bool> OpenGLPipelineLoader::create(Instance &instance, PipelineConfiguration config, CodeGen::ShaderFile file)
    {
        assert(file.mInstances.size() == 2);

        char buffer[256];
#if WINDOWS
        sprintf_s(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#else
        sprintf(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#endif

        Handle pipeline;
        if (!co_await pipeline.create(buffer, {}, [&](OpenGLPipelineLoader *loader, OpenGLPipeline &pipeline, ResourceDataInfo &info) -> Threading::Task<bool> {
                OpenGLShaderLoader::Handle vertexShader;
                if (!co_await vertexShader.create(config.vs, file, VertexShader)) {
                    LOG_ERROR("Failed to load VS '" << config.vs << "'!");
                    co_return false;
                }

                OpenGLShaderLoader::Handle pixelShader;
                if (!co_await pixelShader.create(config.ps, file, PixelShader) && pixelShader) {
                    LOG_ERROR("Failed to load PS '" << config.ps << "'!");
                    co_return false;
                }

                /* OpenGLShaderLoader::Handle geometryShader;
                if (!co_await geometryShader.create(config.gs, GeometryShader) && geometryShader) {
                    LOG_ERROR("Failed to load GS '" << config.gs << "'!");
                    co_return false;
                }*/

                if (!pipeline.link(std::move(vertexShader), {}, std::move(pixelShader))) {
                    LOG_ERROR("Failed to link Program '" << config.vs << "|" << config.gs << "|" << config.ps
                                                         << "'!");
                    co_return false;
                }

                co_return true;
            }))
            co_return false;

        instance = std::make_unique<OpenGLPipelineInstance>(config, std::move(pipeline));

        co_return true;
    }

    Threading::TaskQueue *OpenGLPipelineLoader::loadingTaskQueue() const
    {
        return OpenGLRenderContext::renderQueue();
    }

}
}
