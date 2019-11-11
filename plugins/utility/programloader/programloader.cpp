#include "programloaderlib.h"

#include "programloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"



using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Render::Program, Engine::Resources::ThreadLocalResource>;
METATABLE_BEGIN(LoaderImpl)
MEMBER(mResources)
METATABLE_END(LoaderImpl)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader, LoaderImpl)
METATABLE_END(Engine::Render::ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::ProgramLoader::ResourceType)

RegisterType(Engine::Render::ProgramLoader)

    namespace Engine
{
    namespace Render {

        ProgramLoader::ProgramLoader()
            : VirtualResourceLoaderBase(std::vector<std::string> {})
        {
        }

    }
}