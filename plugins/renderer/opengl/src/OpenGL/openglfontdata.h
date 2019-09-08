#pragma once

#include "util/opengltexture.h"

#include "Modules/math/vector2i.h"

namespace Engine {
namespace Render {

    struct Glyph {
        Vector2i mSize;
        Vector2i mUV;
        int mBearingY;
        int mAdvance;
        bool mFlipped;
    };

    struct MADGINE_OPENGL_EXPORT OpenGLFontData {
        OpenGLTexture mTexture;
        Vector2i mTextureSize;

        std::array<Glyph, 128> mGlyphs;
    };

}
}