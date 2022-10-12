#include "directx11lib.h"

#include "directx11pipelineloader.h"

#include "directx11geometryshaderloader.h"
#include "directx11pixelshaderloader.h"
#include "directx11vertexshaderloader.h"

#include "util/directx11pipelineinstance.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/codegen/codegen_shader.h"

#include "directx11rendercontext.h"

#include "Modules/threading/taskqueue.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11PipelineLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PipelineLoader, Engine::Render::PipelineLoader)
METATABLE_END(Engine::Render::DirectX11PipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PipelineLoader::Resource, Engine::Render::PipelineLoader::Resource)
METATABLE_END(Engine::Render::DirectX11PipelineLoader::Resource)

namespace Engine {
namespace Render {

    DirectX11PipelineLoader::DirectX11PipelineLoader()
    {
    }

    bool DirectX11PipelineLoader::loadImpl(Pipeline &pipeline, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX11PipelineLoader::unloadImpl(Pipeline &pipeline)
    {
        throw 0;
    }

    Threading::Task<bool> DirectX11PipelineLoader::create(Instance &instance, PipelineConfiguration config, bool dynamic)
    {
        DirectX11VertexShaderLoader::Handle vertexShader;
        if (!co_await vertexShader.load(config.vs)) {
            LOG_ERROR("Failed to load VS '" << config.vs << "'!");
            co_return false;
        }
        DirectX11GeometryShaderLoader::Handle geometryShader;
        if (!config.gs.empty() && !co_await geometryShader.load(config.gs)) {
            LOG_ERROR("Failed to load GS '" << config.vs << "'!");
            co_return false;
        }
        DirectX11PixelShaderLoader::Handle pixelShader;
        if (!config.ps.empty() && !co_await pixelShader.load(config.ps)) {
            LOG_ERROR("Failed to load PS '" << config.vs << "'!");
            co_return false;
        }

        instance = std::make_unique<DirectX11PipelineInstance>(config, std::move(vertexShader), std::move(pixelShader), std::move(geometryShader), dynamic);

        co_return true;
    }

    Threading::Task<bool> DirectX11PipelineLoader::create(Instance &instance, PipelineConfiguration config, CodeGen::ShaderFile file)
    {
        assert(file.mInstances.size() == 2);

        DirectX11VertexShaderLoader::Handle vertexShader;
        if (!co_await vertexShader.create(config.vs, file))
            co_return false;
        //DirectX11GeometryShaderLoader::Handle geometryShader;
        //geometryShader.create(name, file);
        DirectX11PixelShaderLoader::Handle pixelShader;
        if (!co_await pixelShader.create(config.ps, file))
            co_return false;

        instance = std::make_unique<DirectX11PipelineInstance>(config, std::move(vertexShader), std::move(pixelShader), DirectX11GeometryShaderLoader::Handle {}, true);

        co_return true;
    }

    Threading::TaskQueue *DirectX11PipelineLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}
