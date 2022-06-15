#include "directx12lib.h"

#include "directx12pipelineloader.h"

#include "directx12pixelshaderloader.h"
#include "directx12vertexshadersourceloader.h"

#include "util/directx12pipelineinstance.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

#include "directx12rendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX12PipelineLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX12PipelineLoader, Engine::Render::PipelineLoader)
METATABLE_END(Engine::Render::DirectX12PipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12PipelineLoader::ResourceType, Engine::Render::PipelineLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX12PipelineLoader::ResourceType)

namespace Engine {
namespace Render {

    DirectX12PipelineLoader::DirectX12PipelineLoader()
    {
    }

    bool DirectX12PipelineLoader::loadImpl(DirectX12Pipeline &pipeline, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX12PipelineLoader::unloadImpl(DirectX12Pipeline &pipeline)
    {
        pipeline.reset();
    }

    Threading::Task<bool> DirectX12PipelineLoader::create(Instance &instance, PipelineConfiguration config, bool dynamic)
    {
        HandleType pipeline;

        char buffer[256];
        sprintf_s(buffer, 256, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());

        if (!co_await pipeline.create(buffer, {}, [&](DirectX12PipelineLoader *loader, DirectX12Pipeline &pipeline, ResourceDataInfo &info) -> Threading::Task<bool> {
                DirectX12GeometryShaderLoader::HandleType geometryShader;
                geometryShader.load(config.gs);
                DirectX12PixelShaderLoader::HandleType pixelShader;
                pixelShader.load(config.ps);
                co_return pipeline.link(config.vs, std::move(geometryShader), std::move(pixelShader));
            }))
            co_return false;

        instance = std::make_unique<DirectX12PipelineInstance>(config, std::move(pipeline));

        co_return true;
    }

    /* bool DirectX12ProgramLoader::create(Program &_program, const std::string &name, const CodeGen::ShaderFile &file)
    {
        assert(file.mInstances.size() == 2);

        DirectX12Program &program = static_cast<DirectX12Program &>(_program);

        DirectX12VertexShaderLoader::HandleType vertexShader;
        vertexShader.create(name, file);
        DirectX12PixelShaderLoader::HandleType pixelShader;
        pixelShader.create(name, file);

        if (!program.link(std::move(vertexShader), std::move(pixelShader)))
            std::terminate();

        return true;
    }*/

    Threading::TaskQueue *DirectX12PipelineLoader::loadingTaskQueue() const
    {
        return DirectX12RenderContext::renderQueue();
    }

}
}
