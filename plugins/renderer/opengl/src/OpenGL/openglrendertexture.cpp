#include "opengllib.h"

#include "openglrendertexture.h"

#include "Generic/bytebuffer.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : OpenGLRenderTarget(context)
        , mTexture(GL_UNSIGNED_BYTE)
        , mSize{ 0, 0 }
    {
        mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        mTexture.setFilter(GL_NEAREST);

        glGenFramebuffers(1, &mFramebuffer);
        GL_CHECK();

        if (config.mCreateDepthBufferView) {
            glGenTextures(1, &mDepthTexture);
            GL_CHECK();
            glBindTexture(GL_TEXTURE_2D, mDepthTexture);
            GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GL_CHECK();
        } else {
            glGenRenderbuffers(1, &mDepthRenderbuffer);
            GL_CHECK();
        }

        resize(size);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture.handle(), 0);        
        GL_CHECK();

        if (config.mCreateDepthBufferView) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
            GL_CHECK();
        } else {
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
            GL_CHECK();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
            GL_CHECK();
        }

#if !OPENGL_ES
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers);
        GL_CHECK();
#endif

        if (GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); check != GL_FRAMEBUFFER_COMPLETE) {
            LOG_ERROR("Incomplete Framebuffer Status: " << check);
            glDump();
            std::terminate();
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GL_CHECK();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();
    }

    OpenGLRenderTexture::~OpenGLRenderTexture()
    {
        glDeleteFramebuffers(1, &mFramebuffer);
        GL_CHECK();
        if (mDepthRenderbuffer) {
            glDeleteRenderbuffers(1, &mDepthRenderbuffer);
            GL_CHECK();
        }
        if (mDepthTexture) {
            glDeleteTextures(1, &mDepthTexture);
        }
    }

    bool OpenGLRenderTexture::resize(const Vector2i &size)
    {
        if (mSize == size)
            return false;

        mSize = size;

        GLsizei width = static_cast<GLsizei>(size.x);
        GLsizei height = static_cast<GLsizei>(size.y);

        assert(width > 0 && height > 0);

        mTexture.setData({ width, height }, {});

        if (mDepthRenderbuffer) {
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
            GL_CHECK();

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
            GL_CHECK();

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            GL_CHECK();
        }
        if (mDepthTexture){
            glBindTexture(GL_TEXTURE_2D, mDepthTexture);
            GL_CHECK();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            GL_CHECK();
        }

        return true;
    }

    void OpenGLRenderTexture::beginFrame()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();

        OpenGLRenderTarget::beginFrame();
    }

    void OpenGLRenderTexture::endFrame()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();
    }

    const OpenGLTexture *OpenGLRenderTexture::texture() const
    {
        return &mTexture;
    }

    TextureHandle OpenGLRenderTexture::depthTexture() const
    {
        return mDepthTexture;
    }

    Vector2i OpenGLRenderTexture::size() const
    {
        return mSize;
    }

}
}
