#include "programloaderlib.h"

#include "programloader.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::ProgramLoader::ResourceType)



    namespace Engine
{
    namespace Render {

        ProgramLoader::ProgramLoader()
            : VirtualResourceLoaderBase(std::vector<std::string> {})
        {
        }

        void ProgramLoader::HandleType::create(const std::string &name, ProgramLoader *loader)
        {
            *this = ProgramLoader::loadManual(
                name, {}, [=](ProgramLoader *loader, Program &program, const ProgramLoader::ResourceType *res) { return loader->create(program, name); }, {},
                loader);
        }

        void ProgramLoader::HandleType::setParameters(const ByteBuffer &data, size_t index, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setParameters(getData(*this, loader), data, index);
        }

        void ProgramLoader::HandleType::setDynamicParameters(const ByteBuffer &data, size_t index, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setDynamicParameters(getData(*this, loader), data, index);
        }

    }
}