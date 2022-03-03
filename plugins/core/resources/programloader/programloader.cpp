#include "programloaderlib.h"

#include "programloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

METATABLE_BEGIN(Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::ProgramLoader::ResourceType)

VIRTUALUNIQUECOMPONENTBASE(Engine::Render::ProgramLoader)

namespace Engine {
namespace Render {

    ProgramLoader::ProgramLoader()
        : VirtualResourceLoaderBase(std::vector<std::string> {})
    {
    }

    Threading::TaskFuture<bool> ProgramLoader::PtrType::create(const std::string &name, const std::vector<size_t> &bufferSizes, size_t instanceDataSize, ProgramLoader *loader)
    {
        return ProgramLoader::loadUnnamed(*this,
            [=](ProgramLoader *loader, Program &program) mutable { return loader->create(program, std::move(name), std::move(bufferSizes), instanceDataSize); }, 
            loader);        
    }

    void ProgramLoader::PtrType::create(const std::string &name, CodeGen::ShaderFile file, ProgramLoader *loader)
    {
        ProgramLoader::loadUnnamed(*this,
            [=, file { std::move(file) }](ProgramLoader *loader, Program &program) mutable { return loader->create(program, std::move(name), file); }, loader);
    }

    void ProgramLoader::PtrType::createUnnamed(CodeGen::ShaderFile &&file, ProgramLoader *loader)
    {
        ProgramLoader::loadUnnamed(
            *this,
            [=, file { std::move(file) }](ProgramLoader *loader, Program &program) mutable { return loader->create(program, Resources::ResourceBase::sUnnamed, file); }, loader);
    }

    WritableByteBuffer ProgramLoader::PtrType::mapParameters(size_t index, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        return loader->mapParameters(**this, index);
    }

    void ProgramLoader::PtrType::setInstanceData(const ByteBuffer &data, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        return loader->setInstanceData(**this, data);
    }

    void ProgramLoader::PtrType::setDynamicParameters(size_t index, const ByteBuffer &data, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        loader->setDynamicParameters(**this, index, data);
    }

}
}