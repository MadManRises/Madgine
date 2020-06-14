#include "imageloaderlib.h"

#include "imageloader.h"

#include "Interfaces/streams/streams.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "imagedata.h"

#include "stb_image.h"

UNIQUECOMPONENT(Engine::Resources::ImageLoader)

METATABLE_BEGIN(Engine::Resources::ImageLoader)
MEMBER(mResources)
METATABLE_END(Engine::Resources::ImageLoader)

METATABLE_BEGIN_BASE(Engine::Resources::ImageLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Resources::ImageLoader::ResourceType)



    namespace Engine
{
    namespace Resources {

        ImageData::ImageData(const std::vector<unsigned char> &buffer)
        {
            mBuffer = stbi_load_from_memory(buffer.data(), buffer.size(),
                &mWidth,
                &mHeight,
                &mChannels,
                STBI_rgb_alpha);
        }

        ImageData::ImageData(ImageData &&other)
            : mBuffer(std::exchange(other.mBuffer, nullptr))
            , mWidth(other.mWidth)
            , mHeight(other.mHeight)
            , mChannels(other.mChannels)
        {
        }

        ImageData::~ImageData()
        {
            clear();
        }

        ImageData &ImageData::operator=(ImageData &&other)
        {
            std::swap(mBuffer, other.mBuffer);
            std::swap(mChannels, other.mChannels);
            std::swap(mWidth, other.mWidth);
            std::swap(mHeight, other.mHeight);
            return *this;
        }

        void ImageData::clear()
        {
            if (mBuffer) {
                stbi_image_free(mBuffer);
                mBuffer = nullptr;
            }
        }

        ImageLoader::ImageLoader()
            : ResourceLoader({ ".png", ".jpg" })
        {
        }

        bool ImageLoader::loadImpl(ImageData &data, ResourceType *res)
        {
            data = { res->readAsBlob() };
            return true;
        }

        void ImageLoader::unloadImpl(ImageData &data, ResourceType *res)
        {
            data.clear();
        }

    }
}