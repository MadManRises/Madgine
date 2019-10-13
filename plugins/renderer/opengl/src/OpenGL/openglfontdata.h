#pragma once

#include "Modules/font/font.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct OpenGLFontData : Font::Font {
        OpenGLFontData()
        {
            mTextureHandle = mTexture.handle();
        }

        OpenGLTexture mTexture;
    };

}
}