#pragma once

#include "Madgine/render/renderwindow.h"

#include "openglshaderprogram.h"

#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    class MADGINE_OPENGL_EXPORT OpenGLRenderWindow : public RenderWindow {
    public:
        OpenGLRenderWindow(Window::Window *w, ContextHandle context, GUI::TopLevelWindow *mTopLevelWindow = nullptr);
        ~OpenGLRenderWindow();

        virtual void render() override;
        virtual void makeCurrent() override;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Scene::Camera *camera, const Vector2 &size) override;

        const ContextHandle mContext;
    private:

        uint32_t mTextureCount = 0;

        OpenGLShaderProgram mProgram;

        OpenGLTexture mDefaultTexture;

		GUI::TopLevelWindow *mTopLevelWindow = nullptr;
    };

}
}