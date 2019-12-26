#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Font {
	
	struct MADGINE_FONTLOADER_EXPORT FontLoader : Resources::VirtualResourceLoaderBase<FontLoader, Font, std::vector<Placeholder<0>>, Threading::WorkGroupStorage> {
        using Resources::VirtualResourceLoaderBase<FontLoader, Font, std::vector<Placeholder<0>>, Threading::WorkGroupStorage>::VirtualResourceLoaderBase;
    };

}
}