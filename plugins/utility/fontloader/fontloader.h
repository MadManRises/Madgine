#pragma once

#include "Modules/resources/resourceloader.h"
#include "Modules/resources/resource.h"

namespace Engine {
namespace Font {
	
	struct MADGINE_FONTLOADER_EXPORT FontLoader : Resources::VirtualResourceLoaderBase<FontLoader, Font, Resources::ThreadLocalResource> {
        using Resources::VirtualResourceLoaderBase<FontLoader, Font, Resources::ThreadLocalResource>::VirtualResourceLoaderBase;
    };

}
}