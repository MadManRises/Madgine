#include "programloaderlib.h"

#include "programloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

METATABLE_BEGIN(Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::ProgramLoader::ResourceType)

namespace Engine {
namespace Render {

    ProgramLoader::ProgramLoader()
        : VirtualResourceLoaderBase(std::vector<std::string> {})
    {
    }

    void ProgramLoader::HandleType::create(const std::string &name, ProgramLoader *loader)
    {
        *this = ProgramLoader::loadManual(
            Resources::ResourceBase::sUnnamed, {}, [=](ProgramLoader *loader, Program &program, const ProgramLoader::ResourceDataInfo &info) { return loader->create(program, name); }, {},
            loader);
    }

    void ProgramLoader::HandleType::create(const std::string &name, CodeGen::ShaderFile &&file, ProgramLoader *loader)
    {
        *this = ProgramLoader::loadManual(
            name, {}, [=, file { std::move(file) }](ProgramLoader *loader, Program &program, const ProgramLoader::ResourceDataInfo &info) mutable { return loader->create(program, name, file); }, {}, loader);
    }

    void ProgramLoader::HandleType::createUnnamed(CodeGen::ShaderFile &&file, ProgramLoader *loader)
    {
        *this = ProgramLoader::loadManual(
            Resources::ResourceBase::sUnnamed, {}, [=, file { std::move(file) }](ProgramLoader *loader, Program &program, const ProgramLoader::ResourceDataInfo &info) mutable { return loader->create(program, Resources::ResourceBase::sUnnamed, file); }, {}, loader);
    }

    void ProgramLoader::HandleType::setParametersSize(size_t index, size_t size, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        loader->setParametersSize(*getDataPtr(*this, loader), index, size);
    }

    WritableByteBuffer ProgramLoader::HandleType::mapParameters(size_t index, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        return loader->mapParameters(*getDataPtr(*this, loader), index);
    }

    void ProgramLoader::HandleType::setInstanceDataSize(size_t size, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        return loader->setInstanceDataSize(*getDataPtr(*this, loader), size);
    }

    void ProgramLoader::HandleType::setInstanceData(const ByteBuffer &data, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        return loader->setInstanceData(*getDataPtr(*this, loader), data);
    }

    void ProgramLoader::HandleType::setDynamicParameters(size_t index, const ByteBuffer &data, ProgramLoader *loader)
    {
        if (!loader)
            loader = &ProgramLoader::getSingleton();
        loader->setDynamicParameters(*getDataPtr(*this, loader), index, data);
    }

}
}