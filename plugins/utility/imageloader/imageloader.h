#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Resources {

	struct ImageData;

	struct MADGINE_IMAGELOADER_EXPORT ImageLoader : ResourceLoader<ImageLoader, ImageData> {
    
		ImageLoader();

		bool loadImpl(ImageData &data, ResourceType *res) override;
        void unloadImpl(ImageData &data) override;

	};


}
}