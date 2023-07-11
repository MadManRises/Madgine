#include "vulkanlib.h"

#include "vulkanpipelineloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "util/vulkanpipelineinstance.h"

#include "Madgine/codegen/codegen_shader.h"

#include "vulkanrendercontext.h"

UNIQUECOMPONENT(Engine::Render::VulkanPipelineLoader);

METATABLE_BEGIN_BASE(Engine::Render::VulkanPipelineLoader, Engine::Render::PipelineLoader)
METATABLE_END(Engine::Render::VulkanPipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::VulkanPipelineLoader::Resource, Engine::Render::PipelineLoader::Resource)
METATABLE_END(Engine::Render::VulkanPipelineLoader::Resource)

namespace Engine {
namespace Render {

    VulkanPipelineLoader::VulkanPipelineLoader()
    {
    }

    bool VulkanPipelineLoader::loadImpl(VulkanPipeline &pipeline, ResourceDataInfo &info)
    {
        throw 0;
    }

    void VulkanPipelineLoader::unloadImpl(VulkanPipeline &pipeline)
    {
        pipeline.reset();
    }

    Threading::Task<bool> VulkanPipelineLoader::create(Instance &instance, PipelineConfiguration config)
    {
        Handle pipeline;

        char buffer[256];
#if WINDOWS
        sprintf_s(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#else
        sprintf(buffer, "%s|%s|%s", config.vs.data(), config.gs.data(), config.ps.data());
#endif

        if (!co_await pipeline.create(buffer, {}, [vs { std::string { config.vs } }, ps { std::string { config.ps } }, gs { std::string { config.gs } }](VulkanPipelineLoader *loader, VulkanPipeline &pipeline, ResourceDataInfo &info) -> Threading::Task<bool> {
                VulkanShaderLoader::Handle vertexShader;
                if (!co_await vertexShader.load(vs, ShaderType::VertexShader)) {
                    LOG_ERROR("Failed to load VS '" << vs << "'!");
                    co_return false;
                }
                VulkanShaderLoader::Handle geometryShader;
                if (!gs.empty() && !co_await geometryShader.load(gs, ShaderType::GeometryShader)) {
                    LOG_ERROR("Failed to load GS '" << gs << "'!");
                    co_return false;
                }
                VulkanShaderLoader::Handle pixelShader;
                if (!ps.empty() && !co_await pixelShader.load(ps, ShaderType::PixelShader)) {
                    LOG_ERROR("Failed to load PS '" << ps << "'!");
                    co_return false;
                }
                co_return pipeline.link(std::move(vertexShader), std::move(geometryShader), std::move(pixelShader));
            }))
            co_return false;

        instance = std::make_unique<VulkanPipelineInstanceHandle>(config, std::move(pipeline));

        co_return true;
    }

    Threading::Task<bool> VulkanPipelineLoader::create(Instance &instance, PipelineConfiguration config, CodeGen::ShaderFile file)
    {
        assert(file.mInstances.size() == 2);

        Ptr pipeline;

        if (!co_await pipeline.create([file { std::move(file) }](VulkanPipelineLoader *loader, VulkanPipeline &pipeline) -> Threading::Task<bool> {
                VulkanShaderLoader::Ptr vertexShader;
                if (!co_await vertexShader.create(file, ShaderType::VertexShader))
                    co_return false;
            /* DirectX12GeometryShaderLoader::Handle geometryShader;
                geometryShader.create(config.gs, file);*/
                VulkanShaderLoader::Ptr pixelShader;
                if (!co_await pixelShader.create(file, ShaderType::PixelShader))
                    co_return false;
                co_return pipeline.link(std::move(vertexShader), {}, std::move(pixelShader));
            }))
            co_return false;

        instance = std::make_unique<VulkanPipelineInstance>(config, std::move(pipeline));

        co_return true;
    }

    Threading::TaskQueue *VulkanPipelineLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }

}
}
