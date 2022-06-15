#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "OpenGL/util/opengltexture.h"

namespace Engine {
namespace Tools {

    struct OpenGLToolConfig : public Tool<OpenGLToolConfig> {

        SERIALIZABLEUNIT(OpenGLToolConfig);

        OpenGLToolConfig(ImRoot &root);
        OpenGLToolConfig(const SceneEditor &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        std::string_view key() const override;

    private:
        Render::OpenGLTexture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::OpenGLToolConfig)