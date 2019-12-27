#pragma once

#include "Modules/resources/resourceloader.h"

#include "imagedata.h"

namespace Engine {
namespace Resources {

	struct MADGINE_IMAGELOADER_EXPORT ImageLoader : ResourceLoader<ImageLoader, ImageData> {
    
		ImageLoader();

		bool loadImpl(ImageData &data, ResourceType *res);
        void unloadImpl(ImageData &data, ResourceType *res);

	};


}
}