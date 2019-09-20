#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Resources {

	struct ImageData;

	struct MADGINE_IMAGELOADER_EXPORT ImageLoader : ResourceLoader<ImageLoader, ImageData> {
    
		ImageLoader();

		std::shared_ptr<ImageData> loadImpl(ResourceType *res) override;

	};


}
}