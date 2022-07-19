#include "pipelineloaderlib.h"

#include "pipelineloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

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

    Threading::TaskFuture<bool> Engine::Render::PipelineLoader::Instance::createGenerated(PipelineConfiguration config, CodeGen::ShaderFile file, PipelineLoader *loader)
    {
        return loader->queueLoading(loader->create(*this, std::move(config), std::move(file)));
    }

    WritableByteBuffer PipelineLoader::Instance::mapParameters(size_t index)
    {
        return mPtr->mapParameters(index);
    }

    void PipelineLoader::Instance::setInstanceData(const ByteBuffer &data)
    {
        mPtr->setInstanceData(data);
    }

    void PipelineLoader::Instance::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        mPtr->setDynamicParameters(index, data);
    }

    void PipelineLoader::Instance::reset()
    {
        mPtr.reset();
    }

    PipelineLoader::Instance::operator PipelineInstance *() const
    {
        return mPtr.get();
    }

}
}