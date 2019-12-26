#pragma once

#include "fontloader.h"

#include "openglfontdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLFontLoader : Resources::VirtualResourceLoaderImpl<OpenGLFontLoader, OpenGLFontData, Font::FontLoader>{
        OpenGLFontLoader();

        bool loadImpl(OpenGLFontData &font, ResourceType *res);
        void unloadImpl(OpenGLFontData &font, ResourceType *res);

    };

}
}