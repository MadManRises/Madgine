#pragma once

#include "Madgine/resources/resourceloader.h"

#include "imagedata.h"

namespace Engine {
namespace Resources {

    struct MADGINE_IMAGELOADER_EXPORT ImageLoader : ResourceLoader<ImageLoader, ImageData> {

        ImageLoader();

        bool loadImpl(ImageData &data, ResourceDataInfo &info);
        void unloadImpl(ImageData &data);
    };

}
}

REGISTER_TYPE(Engine::Resources::ImageLoader)