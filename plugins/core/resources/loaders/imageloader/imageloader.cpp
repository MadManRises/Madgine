#include "imageloaderlib.h"

#include "imageloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "imagedata.h"

#include "stb_image.h"

UNIQUECOMPONENT(Engine::Resources::ImageLoader)

METATABLE_BEGIN(Engine::Resources::ImageLoader)
MEMBER(mResources)
METATABLE_END(Engine::Resources::ImageLoader)

METATABLE_BEGIN_BASE(Engine::Resources::ImageLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Resources::ImageLoader::Resource)

namespace Engine {
namespace Resources {

    ImageData::ImageData(const std::vector<unsigned char> &buffer)
        : mBuffer(stbi_load_from_memory(buffer.data(), buffer.size(),
                      &mWidth,
                      &mHeight,
                      &mChannels,
                      STBI_rgb_alpha),
            stbi_image_free)
    {
    }

    void ImageData::clear()
    {
        mBuffer.reset();
    }

    ImageLoader::ImageLoader()
        : ResourceLoader({ ".png", ".jpg" })
    {
    }

    bool ImageLoader::loadImpl(ImageData &data, ResourceDataInfo &info)
    {
        data = { info.resource()->readAsBlob() };
        return true;
    }

    void ImageLoader::unloadImpl(ImageData &data)
    {
        data.clear();
    }

}
}