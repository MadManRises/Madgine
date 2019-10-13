#pragma once

#include "Modules/math/vector2i.h"

#include "glyph.h"

#include "../resources/resourceloader.h"
#include "../resources/resource.h"

namespace Engine {
namespace Font {

    struct Font {
        uint32_t mTextureHandle;
        Vector2i mTextureSize;

        std::array<Glyph, 128> mGlyphs;
    };

	struct MODULES_EXPORT FontLoader : Resources::VirtualResourceLoaderBase<FontLoader, Font, Resources::ThreadLocalResource> {
		using Resources::VirtualResourceLoaderBase<FontLoader, Font, Resources::ThreadLocalResource>::VirtualResourceLoaderBase;
    };

}
}