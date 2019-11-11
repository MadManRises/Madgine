#pragma once

#include "font.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct OpenGLFontData : Font::Font {
        OpenGLFontData() : mTexture(GL_UNSIGNED_BYTE)
        {
            Font::mTexture = &mTexture;
        }

        OpenGLTexture mTexture;
    };

}
}