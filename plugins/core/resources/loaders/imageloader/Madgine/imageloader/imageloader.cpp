#include "../imageloaderlib.h"

#include "imageloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/threading/awaitables/awaitablesender.h"

#include "imagedata.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

UNIQUECOMPONENT(Engine::Resources::ImageLoader)

METATABLE_BEGIN(Engine::Resources::ImageLoader)
MEMBER(mResources)
METATABLE_END(Engine::Resources::ImageLoader)

METATABLE_BEGIN_BASE(Engine::Resources::ImageLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Resources::ImageLoader::Resource)

namespace Engine {
namespace Resources {

    ImageLoader::ImageLoader()
        : ResourceLoader({ ".png", ".jpg" })
    {
    }

    Threading::Task<bool> ImageLoader::loadImpl(ImageData &data, ResourceDataInfo &info)
    {
        ByteBuffer buffer;
        GenericResult result = (co_await info.resource()->readAsync()).get(buffer);

        stbi_uc *ptr = stbi_load_from_memory(static_cast<const stbi_uc *>(buffer.mData), buffer.mSize,
            &data.mSize.x,
            &data.mSize.y,
            nullptr,
            STBI_rgb_alpha);

        data.mChannels = 4;

        data.mBuffer = { std::unique_ptr<stbi_uc, Functor<&stbi_image_free>> { ptr }, static_cast<size_t>(data.mSize.x) * data.mSize.y * data.mChannels };

        co_return true;
    }

    Threading::Task<void> ImageLoader::unloadImpl(ImageData &data)
    {
        data.mBuffer.clear();
        co_return;
    }

}
}