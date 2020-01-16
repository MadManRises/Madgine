#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "font.h"

namespace Engine {
namespace Render {
	
	struct MADGINE_FONTLOADER_EXPORT FontLoader : Resources::VirtualResourceLoaderBase<FontLoader, Font, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        using Resources::VirtualResourceLoaderBase<FontLoader, Font, std::list<Placeholder<0>>, Threading::WorkGroupStorage>::VirtualResourceLoaderBase;
    };

}
}