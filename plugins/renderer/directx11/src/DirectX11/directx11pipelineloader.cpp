#include "directx11lib.h"

#include "directx11pipelineloader.h"

#include "directx11geometryshaderloader.h"
#include "directx11pixelshaderloader.h"
#include "directx11vertexshaderloader.h"

#include "util/directx11pipelineinstance.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

#include "directx11rendercontext.h"

#include "Modules/threading/taskqueue.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11PipelineLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PipelineLoader, Engine::Render::PipelineLoader)
METATABLE_END(Engine::Render::DirectX11PipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11PipelineLoader::ResourceType, Engine::Render::PipelineLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX11PipelineLoader::ResourceType)

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
        DirectX11VertexShaderLoader::HandleType vertexShader;
        if (!co_await vertexShader.load(config.vs))
            co_return false;
        DirectX11GeometryShaderLoader::HandleType geometryShader;
        if (!co_await geometryShader.load(config.gs) && geometryShader)
            co_return false;
        DirectX11PixelShaderLoader::HandleType pixelShader;
        if (!co_await pixelShader.load(config.ps) && pixelShader)
            co_return false;

        instance = std::make_unique<DirectX11PipelineInstance>(config, std::move(vertexShader), std::move(pixelShader), std::move(geometryShader), dynamic);

        co_return true;
    }

    /* bool DirectX11PipelineLoader::create(Program &_program, const std::string &name, const CodeGen::ShaderFile &file)
    {
        assert(file.mInstances.size() == 2);

        DirectX11Program &program = static_cast<DirectX11Program &>(_program);

        DirectX11VertexShaderLoader::HandleType vertexShader;
        vertexShader.create(name, file);
        DirectX11GeometryShaderLoader::HandleType geometryShader;
        geometryShader.create(name, file);
        DirectX11PixelShaderLoader::HandleType pixelShader;
        pixelShader.create(name, file);

        if (!program.link(std::move(vertexShader), std::move(pixelShader), std::move(geometryShader)))
            std::terminate();

        return true;
    }*/

    Threading::TaskQueue *DirectX11PipelineLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}
