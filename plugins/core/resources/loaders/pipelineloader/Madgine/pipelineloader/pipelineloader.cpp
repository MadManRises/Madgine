#include "../pipelineloaderlib.h"

#include "pipelineloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/codegen/codegen_shader.h"

#include "Madgine/meshloaderlib.h"
#include "Madgine/meshloader/gpumeshloader.h"

METATABLE_BEGIN(Engine::Render::PipelineLoader)
METATABLE_END(Engine::Render::PipelineLoader)

METATABLE_BEGIN_BASE(Engine::Render::PipelineLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::PipelineLoader::Resource)

VIRTUALUNIQUECOMPONENTBASE(Engine::Render::PipelineLoader)

namespace Engine {
namespace Render {

    PipelineLoader::PipelineLoader()
        : VirtualResourceLoaderBase(std::vector<std::string> {})
    {
    }

    PipelineLoader::Instance::Instance(std::unique_ptr<PipelineInstance> ptr)
        : mPtr(std::move(ptr))
    {
    }

    PipelineLoader::Instance &PipelineLoader::Instance::operator=(std::unique_ptr<PipelineInstance> ptr)
    {
        std::swap(mPtr, ptr);
        return *this;
    }

    Threading::TaskFuture<bool> PipelineLoader::Instance::createStatic(PipelineConfiguration config, PipelineLoader *loader)
    {
        return loader->queueLoading(loader->create(*this, std::move(config), false));
    }

    Threading::TaskFuture<bool> PipelineLoader::Instance::createDynamic(PipelineConfiguration config, PipelineLoader *loader)
    {
        return loader->queueLoading(loader->create(*this, std::move(config), true));
    }

    Threading::TaskFuture<bool> PipelineLoader::Instance::createGenerated(PipelineConfiguration config, CodeGen::ShaderFile file, PipelineLoader *loader)
    {
        return loader->queueLoading(loader->create(*this, std::move(config), std::move(file)));
    }

        void PipelineLoader::Instance::reset()
    {
        mPtr.reset();
    }

    PipelineLoader::Instance::operator PipelineInstance *() const
    {
        return mPtr.get();
    }

    PipelineInstance *PipelineLoader::Instance::operator->() const
    {
        return mPtr.get();
    }
    
    void PipelineInstance::renderQuad() const
    {
        static GPUMeshLoader::Handle quad = GPUMeshLoader::loadManual("quad", {}, [](Render::GPUMeshLoader *loader, Render::GPUMeshData &data, Render::GPUMeshLoader::ResourceDataInfo &info) {
            std::vector<Compound<Render::VertexPos_3D>> vertices {
                { { -1, -1, 0 } },
                { { 1, -1, 0 } },
                { { -1, 1, 0 } },
                { { 1, 1, 0 } }
            };

            std::vector<uint32_t> indices {
                0, 1, 2, 1, 2, 3
            };

            return loader->generate(data, { 3, std::move(vertices), std::move(indices) });
        });

        if (quad.available())
            renderMesh(quad);
    }


}
}