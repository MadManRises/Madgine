#include "opengllib.h"

#include "openglrendertexture.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size)
        : OpenGLRenderTarget(context)
        , mTexture(GL_UNSIGNED_BYTE)
        , mSize{ 0, 0 }
    {
        mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        mTexture.setFilter(GL_NEAREST);

        glGenRenderbuffers(1, &mDepthRenderbuffer);
        GL_CHECK();

        glGenFramebuffers(1, &mFramebuffer);
        GL_CHECK();

        resize(size);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture.handle(), 0);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexture.handle(), 0);
        GL_CHECK();

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
        glDeleteRenderbuffers(1, &mDepthRenderbuffer);
        GL_CHECK();
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

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        GL_CHECK();

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GL_CHECK();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();

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

    Vector2i OpenGLRenderTexture::size() const
    {
        return mSize;
    }

}
}
