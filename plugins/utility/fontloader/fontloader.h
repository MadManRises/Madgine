#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "font.h"

namespace Engine {
namespace Render {
	
	struct MADGINE_FONTLOADER_EXPORT FontLoader : Resources::ResourceLoader<FontLoader, Font, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        FontLoader();

		bool loadImpl(Font &font, ResourceType *res);
        void unloadImpl(Font &font, ResourceType *res);
    };

}
}

RegisterType(Engine::Render::FontLoader);