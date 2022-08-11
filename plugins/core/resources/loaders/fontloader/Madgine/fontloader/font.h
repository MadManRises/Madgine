#pragma once

#include "Meta/math/vector2i.h"

#include "Madgine/textureloader/textureloader.h"

#include "glyph.h"

namespace Engine {
namespace Render {

    struct Font {
        TextureLoader::Ptr mTexture;
        //TextureHandle *mTextureHandle;
        Vector2i mTextureSize;

        std::array<Glyph, 128> mGlyphs;

        int mAscender;
        int mDescender;
    };

}
}