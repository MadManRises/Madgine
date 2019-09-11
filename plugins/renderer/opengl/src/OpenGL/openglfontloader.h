#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLFontLoader : Resources::ResourceLoader<OpenGLFontLoader, OpenGLFontData, Resources::ThreadLocalResource> {
        OpenGLFontLoader();

        std::shared_ptr<OpenGLFontData> loadImpl(ResourceType *res) override;

    };

}
}