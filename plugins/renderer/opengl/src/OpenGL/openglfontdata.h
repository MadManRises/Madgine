#pragma once

#include "util/opengltexture.h"

#include "Modules/math/vector2i.h"

#include "Modules/font/glyph.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLFontData {
        OpenGLTexture mTexture;
        Vector2i mTextureSize;

        std::array<Font::Glyph, 128> mGlyphs;
    };

}
}