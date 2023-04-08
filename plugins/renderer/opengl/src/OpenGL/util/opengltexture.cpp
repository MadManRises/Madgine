#include "../opengllib.h"

#include "opengltexture.h"

#include "Generic/areaview.h"
#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLTexture::OpenGLTexture(TextureType type, DataFormat format, size_t samples)
        : Texture(type, format)
        , mSamples(samples)
    {
        GLuint temp;
        glGenTextures(1, &temp);
        mTextureHandle = temp;
        GL_CHECK();
        if (mType != TextureType_2DMultiSample)
            setFilter(GL_LINEAR);
        bind();
#if !OPENGL_ES
        if (mType != TextureType_2DMultiSample) {
            glTexParameteri(target(), GL_TEXTURE_BASE_LEVEL, 0);
            GL_CHECK();
            glTexParameteri(target(), GL_TEXTURE_MAX_LEVEL, 0);
            GL_CHECK();
        }
#endif
    }

    OpenGLTexture::~OpenGLTexture()
    {
        reset();
    }

    OpenGLTexture &OpenGLTexture::operator=(OpenGLTexture &&other)
    {
        Texture::operator=(std::move(other));
        return *this;
    }

    void OpenGLTexture::reset()
    {
        if (mTextureHandle) {
            GLuint temp = mTextureHandle;
            glDeleteTextures(1, &temp);
            GL_CHECK();
            mTextureHandle = 0;
        }
    }

    void OpenGLTexture::bind() const
    {
        glBindTexture(target(), mTextureHandle);
        GL_CHECK();
    }

    void OpenGLTexture::setData(Vector2i size, const ByteBuffer &data)
    {
        GLenum internalStorage;
        GLenum internalFormat;
        GLenum sizedFormat;
        size_t byteWidth;
        switch (mFormat) {
        case FORMAT_RGBA8:
            internalStorage = GL_UNSIGNED_BYTE;
            internalFormat = GL_RGBA;
            sizedFormat = GL_RGBA8;
            byteWidth = 4;
            break;
        case FORMAT_RGBA16F:
            internalStorage = GL_FLOAT;
            internalFormat = GL_RGBA;
            sizedFormat = GL_RGBA16F;
            assert(!data.mData);
            break;
        case FORMAT_R32F:
            internalStorage = GL_FLOAT,
            internalFormat = GL_RED;
            sizedFormat = GL_R32F;
            break;
        case FORMAT_D24:
            internalStorage = GL_UNSIGNED_INT;
            internalFormat = GL_DEPTH_COMPONENT;
            sizedFormat = GL_DEPTH_COMPONENT24;
            assert(!data.mData);
            break;
        case FORMAT_D32:
            internalStorage = GL_FLOAT;
            internalFormat = GL_DEPTH_COMPONENT;
            sizedFormat = GL_DEPTH_COMPONENT32F;
            assert(!data.mData);
            break;
        default:
            throw 0;
        }
        bind();
        switch (mType) {
        case TextureType_2D:
            glTexImage2D(target(), 0, sizedFormat, size.x, size.y, 0, internalFormat, internalStorage, data.mData);
            break;
        case TextureType_2DMultiSample:
#if OPENGL_ES
#    if OPENGL_ES >= 31
            glTexStorage2DMultisample(target(), mSamples, sizedFormat, size.x, size.y, true);
#    else
            throw 0;
#    endif
#else
            glTexImage2DMultisample(target(), mSamples, sizedFormat, size.x, size.y, true);
#endif
            break;
        case TextureType_Cube:
            for (int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, sizedFormat, size.x, size.y, 0, internalFormat, internalStorage, data.mData);
            break;
        }
        GL_CHECK();
        mSize = size;
    }

    void OpenGLTexture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        GLenum internalStorage;
        GLenum internalFormat;
        GLenum sizedFormat;
        size_t byteWidth;
        switch (mFormat) {
        case FORMAT_RGBA8:
            internalStorage = GL_UNSIGNED_BYTE;
            internalFormat = GL_RGBA;
            sizedFormat = GL_RGBA8;
            byteWidth = 4;
            break;
        case FORMAT_RGBA16F:
            internalStorage = GL_FLOAT;
            internalFormat = GL_RGBA;
            sizedFormat = GL_RGBA16F;
            break;
        case FORMAT_R32F:
            internalStorage = GL_FLOAT,
            internalFormat = GL_RED;
            sizedFormat = GL_R32F;
            break;
        case FORMAT_D24:
            internalStorage = GL_UNSIGNED_INT;
            internalFormat = GL_DEPTH_COMPONENT;
            sizedFormat = GL_DEPTH_COMPONENT24;
            break;
        default:
            throw 0;
        }
        bind();
        glTexSubImage2D(target(), 0, offset.x, offset.y, size.x, size.y, internalFormat, internalStorage, data.mData);
        GL_CHECK();
    }

    void OpenGLTexture::setWrapMode(GLint mode)
    {
        bind();
        glTexParameteri(target(), GL_TEXTURE_WRAP_S, mode);
        GL_CHECK();
        glTexParameteri(target(), GL_TEXTURE_WRAP_T, mode);
        GL_CHECK();
    }

    void OpenGLTexture::setFilter(GLint filter)
    {
        assert(mType != TextureType_2DMultiSample);
        bind();
        glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, filter);
        GL_CHECK();
        glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, filter);
        GL_CHECK();
    }

    GLenum OpenGLTexture::target() const
    {
        switch (mType) {
        case TextureType_2D:
            return GL_TEXTURE_2D;
#if MULTISAMPLING
        case TextureType_2DMultiSample:
            return GL_TEXTURE_2D_MULTISAMPLE;
#endif
        case TextureType_Cube:
            return GL_TEXTURE_CUBE_MAP;
        default:
            throw 0;
        }
    }

    OpenGLTexture::operator bool() const
    {
        return mTextureHandle != 0;
    }

}
}
