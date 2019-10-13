#pragma once

#include "Modules/font/font.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLFontLoader : VirtualResourceLoaderImpl<OpenGLFontLoader, Font::FontLoader>{
        OpenGLFontLoader();

        std::shared_ptr<Font::Font> loadImpl(ResourceType *res) override;

    };

}
}