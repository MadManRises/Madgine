#pragma once

#include "fontloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLFontLoader : Resources::VirtualResourceLoaderImpl<OpenGLFontLoader, Font::FontLoader>{
        OpenGLFontLoader();

        std::shared_ptr<Font::Font> loadImpl(ResourceType *res) override;

    };

}
}