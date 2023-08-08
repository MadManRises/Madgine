#include "opengllib.h"

#include "openglrendertexture.h"

#include "Madgine/render/rendertextureconfig.h"

#include "Generic/bytebuffer.h"

#include "openglrendercontext.h"

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    GLenum toGLFormat(DataFormat format)
    {
        switch (format) {
        case FORMAT_RGBA8:
            return GL_RGBA8;
        case FORMAT_RGBA16F:
            return GL_RGBA16F;
        case FORMAT_R32F:
            return GL_R32F;
        default:
            throw 0;
        }
    }

    void attachFramebufferTexture(GLenum attachment, OpenGLTexture &tex, size_t j)
    {
#if CUBE_FRAMEBUFFER
#    if !OPENGL_ES
        if (glFramebufferTexture) {
#    endif
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, tex.handle(), 0);
            GL_CHECK();
#    if !OPENGL_ES
        } else
#    endif
#endif
#if OPENGL_ES < 32
        {
            GLenum target = tex.target();
            if (target == GL_TEXTURE_CUBE_MAP) {
                target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + j;
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex.handle(), 0);
            GL_CHECK();
        }
#endif
    }

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : OpenGLRenderTarget(context, false, config.mName, config.mIterations, config.mBlitSource)
        , mSamples(context->supportsMultisampling() ? config.mSamples : 1)
        , mSize { 0, 0 }
        , mType(config.mType)
    {
        if (mType == TextureType_2DMultiSample && !context->supportsMultisampling())
            mType = TextureType_2D;

        bool createDepthBufferView = config.mCreateDepthBufferView;

        size_t bufferCount = config.mIterations > 1 ? 2 : 1;
        size_t framebufferCount = getFramebufferCount(&createDepthBufferView);


        glGenFramebuffers(framebufferCount, mFramebuffers);
        GL_CHECK();

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            OpenGLTexture &tex = mTextures.emplace_back(mType, config.mFormat, config.mSamples);
            if (mType != TextureType_2DMultiSample) {
                tex.setWrapMode(GL_CLAMP_TO_EDGE);
                tex.setFilter(GL_NEAREST);
            }
        }

        if (createDepthBufferView) {
            mDepthTexture = { mType, FORMAT_D32, mSamples };
            if (mType == TextureType_Cube) {
                mDepthTexture.setWrapMode(GL_CLAMP_TO_EDGE);
                mDepthTexture.setFilter(GL_NEAREST);
            }
        } else {
            glGenRenderbuffers(1, &mDepthRenderbuffer);
            GL_CHECK();
        }

        resize(size);

        for (size_t j = 0; j < framebufferCount; ++j) {

            glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[j]);
            GL_CHECK();

            GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            assert(config.mTextureCount <= array_size(DrawBuffers));
            glDrawBuffers(config.mTextureCount, DrawBuffers);
            GL_CHECK();

            if (GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); check != GL_FRAMEBUFFER_COMPLETE) {
                if (check == 0) {
                    GL_CHECK();
                } else {
                    LOG_ERROR("Incomplete Framebuffer Status: " << check << " (depth-texture: " << createDepthBufferView << ")");
                    glDump();
                }
                std::terminate();
            }

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            GL_CHECK();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GL_CHECK();
        }
    }

    OpenGLRenderTexture::~OpenGLRenderTexture()
    {
        glDeleteFramebuffers(mFramebuffers[1] != 0 ? 2 : 1, mFramebuffers);
        GL_CHECK();
        if (mDepthRenderbuffer) {
            glDeleteRenderbuffers(1, &mDepthRenderbuffer);
            GL_CHECK();
        }
    }

    bool OpenGLRenderTexture::resizeImpl(const Vector2i &size)
    {
        if (mSize == size)
            return false;

        mSize = size;

        GLsizei width = static_cast<GLsizei>(size.x);
        GLsizei height = static_cast<GLsizei>(size.y);

        assert(width > 0 && height > 0);

        for (OpenGLTexture &tex : mTextures)
            tex.setData({ width, height }, {});

        if (mDepthRenderbuffer) {
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
            GL_CHECK();

            if (mSamples > 1)
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT24, width, height);
            else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            GL_CHECK();

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            GL_CHECK();
        }
        if (mDepthTexture) {
            mDepthTexture.setData({ width, height }, {});
        }

        size_t bufferCount = iterations() > 1 ? 2 : 1;
        size_t framebufferCount = getFramebufferCount();
        for (size_t j = 0; j < framebufferCount; ++j) {

            glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[j]);
            GL_CHECK();
            for (size_t i = 0; i < mTextures.size() / bufferCount; ++i) {
                OpenGLTexture &tex = mTextures[i * bufferCount + j];
                attachFramebufferTexture(GL_COLOR_ATTACHMENT0 + i, tex, i);
            }

            if (mDepthTexture) {
                attachFramebufferTexture(GL_DEPTH_ATTACHMENT, mDepthTexture, 0);
            } else {
                glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
                GL_CHECK();
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
                GL_CHECK();
            }
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GL_CHECK();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();

        return true;
    }

    void OpenGLRenderTexture::beginIteration(size_t iteration) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[iteration % 2]);
        GL_CHECK();

        OpenGLRenderTarget::beginIteration(iteration);

        if (mBlitSource)
            blit(mBlitSource);
    }

    void OpenGLRenderTexture::endIteration(size_t iteration) const
    {
        OpenGLRenderTarget::endIteration(iteration);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();
    }

    TextureDescriptor OpenGLRenderTexture::texture(size_t index, size_t iteration) const
    {
        if (iteration == std::numeric_limits<size_t>::max())
            iteration = iterations();

        int bufferCount = iterations() > 1 ? 2 : 1;
        int offset = iterations() > 1 ? 1 - iteration % 2 : 0;
        return mTextures[bufferCount * index + offset].descriptor();
    }

    size_t OpenGLRenderTexture::textureCount() const
    {
        return mTextures.size();
    }

    TextureDescriptor OpenGLRenderTexture::depthTexture() const
    {
        return mDepthTexture.descriptor();
    }

    void OpenGLRenderTexture::blit(RenderTarget *input) const
    {
        OpenGLRenderTexture *inputTex = dynamic_cast<OpenGLRenderTexture *>(input);
        assert(inputTex);

        size_t count = std::min(mTextures.size(), inputTex->mTextures.size());

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffers[0]);
        GL_CHECK();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, inputTex->mFramebuffers[0]);
        GL_CHECK();

        for (size_t i = 0; i < mTextures.size(); ++i) {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            GL_CHECK();
            GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
            glDrawBuffers(1, &attachment);
            GL_CHECK();
            glBlitFramebuffer(0, 0, inputTex->mSize.x, inputTex->mSize.y, 0, 0, mSize.x, mSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            GL_CHECK();
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        GL_CHECK();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        GL_CHECK();
    }

    Matrix4 OpenGLRenderTexture::getClipSpaceMatrix() const
    {
        return Matrix4 { 1, 0, 0, 0,
            0, -1, 0, 0,
            0, 0, 2, -1,
            0, 0, 0, 1 };
    }

    size_t OpenGLRenderTexture::getFramebufferCount(bool *emulateCube) const
    {
        size_t count = iterations() > 1 ? 2 : 1;

#if OPENGL_ES < 32
#    if !OPENGL_ES
        if (!glFramebufferTexture)
#    endif
        {
            if (mType == TextureType_Cube) {
                assert(count == 1);
                count = 6;                
                if (emulateCube)
                    *emulateCube = true;
            }
        }
#endif
        return count;
    }

    Vector2i OpenGLRenderTexture::size() const
    {
        return mSize;
    }
}
}
