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

    void attachFramebufferTexture(GLenum attachment, OpenGLTexture &tex, TextureType type, size_t j)
    {
#if CUBE_FRAMEBUFFER
#if !OPENGL_ES
        if (glFramebufferTexture){
#endif
            assert(j == 0);
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, tex.handle(), 0);
            GL_CHECK();
#if !OPENGL_ES
        }
        else
#endif
#endif
#if OPENGL_ES < 32
        {
            GLenum target = tex.target();
            if (type == TextureType_Cube) {
                target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + j;
            } else {
                assert(j == 0);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, tex.handle(), 0);
            GL_CHECK();
        }
#endif
    }

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : OpenGLRenderTarget(context, false, config.mName, config.mIterations)
        , mSamples(context->supportsMultisampling() ? config.mSamples : 1)
        , mSize { 0, 0 }
    {
        TextureType type = config.mType;
        if (type == TextureType_2DMultiSample && !context->supportsMultisampling())
            type = TextureType_2D;

        bool createDepthBufferView = config.mCreateDepthBufferView;

        size_t bufferCount = config.mIterations > 1 && mSamples == 1 ? 2 : 1;
        size_t framebufferCount = bufferCount;
#if OPENGL_ES < 32
#if !OPENGL_ES
        if (!glFramebufferTexture)
#endif
        {
            if (type == TextureType_Cube) {
                assert(framebufferCount == 1);
                framebufferCount = 6;
                createDepthBufferView = true;
            }
        }
#endif

        glGenFramebuffers(framebufferCount, mFramebuffers);
        GL_CHECK();

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            OpenGLTexture &tex = mTextures.emplace_back(type, config.mFormat);
            tex.setWrapMode(GL_CLAMP_TO_EDGE);
            tex.setFilter(GL_NEAREST);
        }

        if (createDepthBufferView) {
            mDepthTexture = { type, FORMAT_D32, mSamples };
            if (type == TextureType_Cube) {
                mDepthTexture.setWrapMode(GL_CLAMP_TO_EDGE);
                mDepthTexture.setFilter(GL_NEAREST);
            }
        } else {
            glGenRenderbuffers(1, &mDepthRenderbuffer);
            GL_CHECK();
        }

        if (mSamples > 1) {
            mMultisampledTextures.resize(config.mTextureCount);
            glGenTextures(config.mTextureCount, mMultisampledTextures.data());
            GL_CHECK();
            glGenFramebuffers(1, &mMultisampledFramebuffer);
            GL_CHECK();
        }

        resize(size);

        for (size_t j = 0; j < framebufferCount; ++j) {

#if MULTISAMPLING
            if (mSamples > 1) {
                glBindFramebuffer(GL_FRAMEBUFFER, mMultisampledFramebuffer);
                GL_CHECK();
                for (size_t i = 0; i < config.mTextureCount; ++i) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mTextures[i].handle(), 0);
                    GL_CHECK();
                }
                glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[0]);
                GL_CHECK();
                for (size_t i = 0; i < config.mTextureCount; ++i) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, mMultisampledTextures[i], 0);
                    GL_CHECK();
                }
            } else
#endif
            {
                glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[j]);
                GL_CHECK();
                for (size_t i = 0; i < config.mTextureCount; ++i) {
                    attachFramebufferTexture(GL_COLOR_ATTACHMENT0 + i, mTextures[i * bufferCount + j], type, j);
                }
            }

            if (createDepthBufferView) {
                attachFramebufferTexture(GL_DEPTH_ATTACHMENT, mDepthTexture, type, j);
            } else {
                glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
                GL_CHECK();
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
                GL_CHECK();
            }

            GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            assert(config.mTextureCount <= array_size(DrawBuffers));
            glDrawBuffers(config.mTextureCount, DrawBuffers);
            GL_CHECK();

            if (GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); check != GL_FRAMEBUFFER_COMPLETE) {
                if (check == 0){
                    GL_CHECK();
                }else{
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
        if (mMultisampledFramebuffer) {
            glDeleteFramebuffers(1, &mMultisampledFramebuffer);
            GL_CHECK();
        }
        if (!mMultisampledTextures.empty()) {
            glDeleteTextures(mMultisampledTextures.size(), mMultisampledTextures.data());
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

#if MULTISAMPLING
        for (size_t i = 0; i < mMultisampledTextures.size(); ++i) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mMultisampledTextures[i]);
            GL_CHECK();
#if OPENGL_ES
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, mHDR ? GL_RGBA16F : GL_RGBA, width, height, GL_TRUE);
#else
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, mHDR ? GL_RGBA16F : GL_RGBA, width, height, GL_TRUE);
#endif
            GL_CHECK();
        }
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        GL_CHECK();
#endif

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

        return true;
    }

    void OpenGLRenderTexture::beginIteration(size_t iteration) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[iteration % 2]);
        GL_CHECK();

        OpenGLRenderTarget::beginIteration(iteration);
    }

    void OpenGLRenderTexture::endIteration(size_t iteration) const
    {
        OpenGLRenderTarget::endIteration(iteration);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();

        if (mSamples > 1) {
            for (size_t i = 0; i < mTextures.size(); ++i) {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebuffers[0]);
                GL_CHECK();
                glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
                GL_CHECK();
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mMultisampledFramebuffer);
                GL_CHECK();
                GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
                glDrawBuffers(1, &attachment);
                GL_CHECK();
                glBlitFramebuffer(0, 0, mSize.x, mSize.y, 0, 0, mSize.x, mSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                GL_CHECK();
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                GL_CHECK();
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                GL_CHECK();
            }
        }

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

    Matrix4 OpenGLRenderTexture::getClipSpaceMatrix() const
    {
        return Matrix4 { 1, 0, 0, 0,
            0, -1, 0, 0,
            0, 0, 2, -1,
            0, 0, 0, 1 };
    }

    Vector2i OpenGLRenderTexture::size() const
    {
        return mSize;
    }
}
}
