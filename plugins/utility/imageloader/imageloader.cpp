#include "imageloaderlib.h"

#include "imageloader.h"

#include "Interfaces/streams/streams.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "imagedata.h"

#include "stb_image.h"

UNIQUECOMPONENT(Engine::Resources::ImageLoader)

METATABLE_BEGIN(Engine::Resources::ImageLoader)
MEMBER(mResources)
METATABLE_END(Engine::Resources::ImageLoader)

METATABLE_BEGIN_BASE(Engine::Resources::ImageLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Resources::ImageLoader::ResourceType)

RegisterType(Engine::Resources::ImageLoader)

    namespace Engine
{
    namespace Resources {

        ImageData::ImageData(const char *path)
        {
            mBuffer = stbi_load(path,
                &mWidth,
                &mHeight,
                &mChannels,
                STBI_rgb_alpha);
        }

        ImageData::~ImageData()
        {
            stbi_image_free(mBuffer);
        }

        ImageLoader::ImageLoader()
            : ResourceLoader({ ".png" })
        {
        }

        std::shared_ptr<ImageData> ImageLoader::loadImpl(ResourceType *res)
        {
            return std::make_shared<ImageData>(res->path().c_str());            
        }

    }
}