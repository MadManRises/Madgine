#pragma once

#include "Modules/math/vector2i.h"

#include "glyph.h"

namespace Engine {
namespace Render {
    struct Texture;
}
namespace Font {

    struct Font {
        Render::Texture *mTexture;
        Vector2i mTextureSize;

        std::array<Glyph, 128> mGlyphs;
    };

}
}