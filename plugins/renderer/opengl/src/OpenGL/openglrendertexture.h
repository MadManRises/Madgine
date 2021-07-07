#pragma once

#include "openglrendertarget.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTexture : OpenGLRenderTarget {

        OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~OpenGLRenderTexture();

        bool resize(const Vector2i &size) override;
        Vector2i size() const override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual const OpenGLTexture *texture() const override;

        virtual TextureHandle depthTexture() const override;

    private:
        GLuint mFramebuffer;
        GLuint mDepthRenderbuffer = 0;
        GLuint mDepthTexture = 0;

        OpenGLTexture mTexture;

		Vector2i mSize;
    };

}
}