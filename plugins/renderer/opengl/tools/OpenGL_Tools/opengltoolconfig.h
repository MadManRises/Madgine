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
        virtual void finalize() override;

        std::string_view key() const override;

    private:
        Render::OpenGLTexture mImageTexture;
    };

}
}

RegisterType(Engine::Tools::OpenGLToolConfig);