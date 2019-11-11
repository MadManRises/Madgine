#include "textureloaderlib.h"

#include "textureloader.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "texture.h"


using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Render::Texture, Engine::Resources::ThreadLocalResource>;
METATABLE_BEGIN(LoaderImpl)
MEMBER(mResources)
METATABLE_END(LoaderImpl)

METATABLE_BEGIN_BASE(Engine::Render::TextureLoader, LoaderImpl)
METATABLE_END(Engine::Render::TextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::TextureLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::TextureLoader::ResourceType)

RegisterType(Engine::Render::TextureLoader)

    namespace Engine
{
    namespace Render {

        TextureLoader::TextureLoader()
            : VirtualResourceLoaderBase(std::vector<std::string>{ })
        {
        }

        
    }
}