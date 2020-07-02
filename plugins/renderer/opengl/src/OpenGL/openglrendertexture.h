#pragma once

#include "openglrendertarget.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTexture : OpenGLRenderTarget {

        OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size);
        ~OpenGLRenderTexture();

        bool resize(const Vector2i &size) override;
        Vector2i size() const override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual const OpenGLTexture *texture() const override;

    private:
        GLuint mFramebuffer;
        GLuint mDepthRenderbuffer;

        OpenGLTexture mTexture;

		Vector2i mSize;
    };

}
}