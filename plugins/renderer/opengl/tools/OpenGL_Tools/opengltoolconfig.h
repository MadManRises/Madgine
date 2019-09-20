#pragma once

#include "toolscollector.h"

#include "OpenGL/util/opengltexture.h"

namespace Engine {
namespace Tools {

    struct OpenGLToolConfig : public Tool<OpenGLToolConfig> {

        SERIALIZABLEUNIT;

        OpenGLToolConfig(ImRoot &root);
        OpenGLToolConfig(const SceneEditor &) = delete;

        virtual bool init() override;

        const char *key() const override;

    private:
        Render::OpenGLTexture mImageTexture;
    };

}
}