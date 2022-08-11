#include "../imageloaderlib.h"

#include "imageloader.h"

#include "Meta/keyvalue/metatable_impl.h"

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

    bool ImageLoader::loadImpl(ImageData &data, ResourceDataInfo &info)
    {
        std::vector<unsigned char> buffer = info.resource()->readAsBlob();

        stbi_uc *ptr = stbi_load_from_memory(buffer.data(), buffer.size(),
            &data.mSize.x,
            &data.mSize.y,
            &data.mChannels,
            STBI_rgb_alpha);

        data.mBuffer = { std::unique_ptr<stbi_uc, Functor<&stbi_image_free>> { ptr }, static_cast<size_t>(data.mSize.x) * data.mSize.y * data.mChannels };

        return true;
    }

    void ImageLoader::unloadImpl(ImageData &data)
    {
        data.mBuffer.clear();
    }

}
}