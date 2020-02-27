#pragma once

#include "Modules/math/vector2i.h"

#include "textureloader.h"

#include "glyph.h"

namespace Engine {
namespace Render {

    struct Font {
        TextureLoader::HandleType mTexture;
        TextureHandle mTextureHandle;
        Vector2i mTextureSize;

        std::array<Glyph, 128> mGlyphs;
    };

}
}