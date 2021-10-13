#include "../opengllib.h"

#include "opengltexture.h"

#include "Generic/areaview.h"
#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLTexture::OpenGLTexture(TextureType type, DataFormat format, size_t samples)
        : mType(type)
        , mFormat(format)
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
        glTexParameteri(target(), GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target(), GL_TEXTURE_MAX_LEVEL, 0);
        GL_CHECK();
#endif
    }

    OpenGLTexture::OpenGLTexture(OpenGLTexture &&other)
        : mType(other.mType)
        , mFormat(other.mFormat)
    {
        mTextureHandle = std::exchange(other.mTextureHandle, 0);
    }

    OpenGLTexture::~OpenGLTexture()
    {
        reset();
    }

    OpenGLTexture &OpenGLTexture::operator=(OpenGLTexture &&other)
    {
        std::swap(mTextureHandle, other.mTextureHandle);
        std::swap(mType, other.mType);
        std::swap(mFormat, other.mFormat);
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
        const void *ptr = data.mData;
        std::vector<unsigned char> flippedData;
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
            assert(!ptr);
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
            assert(!ptr);
            break;
        case FORMAT_D32:
            internalStorage = GL_FLOAT;
            internalFormat = GL_DEPTH_COMPONENT;
            sizedFormat = GL_DEPTH_COMPONENT32F;
            assert(!ptr);
            break;
        default:
            throw 0;
        }
        if (ptr) {
            flippedData.reserve(size.x * size.y * byteWidth);
            AreaView<const unsigned char, 2> view { static_cast<const unsigned char *>(data.mData), { size.x * byteWidth, static_cast<size_t>(size.y) } };
            view.flip(1);
            std::copy(view.begin(), view.end(), std::back_inserter(flippedData));
            ptr = flippedData.data();
        }
        bind();
        switch (mType) {
        case TextureType_2D:
            glTexImage2D(target(), 0, sizedFormat, size.x, size.y, 0, internalFormat, internalStorage, ptr);
            break;
        case TextureType_2DMultiSample:
            glTexImage2DMultisample(target(), mSamples, sizedFormat, size.x, size.y, true);
            break;
        case TextureType_Cube:
            for (int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, sizedFormat, size.x, size.y, 0, internalFormat, internalStorage, ptr);
            break;
        }
        GL_CHECK();
        mSize = size;
    }

    void OpenGLTexture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        std::vector<unsigned char> flippedData;
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
        flippedData.reserve(size.x * size.y * byteWidth);
        AreaView<const unsigned char, 2> view { static_cast<const unsigned char *>(data.mData), { size.x * byteWidth, static_cast<size_t>(size.y) } };
        view.flip(1);
        std::copy(view.begin(), view.end(), std::back_inserter(flippedData));
        bind();
        glTexSubImage2D(target(), 0, offset.x, mSize.y - offset.y - size.y, size.x, size.y, internalFormat, internalStorage, flippedData.data());
        GL_CHECK();
    }

    void OpenGLTexture::resize(Vector2i size)
    {
        /* Vector2i commonSize = min(size, mSize);
        GLuint tempTex;
        TextureHandle helper;
        glGenTextures(1, &tempTex);
        GL_CHECK();
        helper = tempTex;
        std::swap(helper, mTextureHandle);
        tempTex = helper;
        */
        setData(size, {});

        /* glCopyImageSubData(tempTex, GL_TEXTURE_2D, 0, 0, 0, 0, mTextureHandle, GL_TEXTURE_2D, 0, 0, 0, 0, commonSize.x, commonSize.y, 1);
        GL_CHECK();

        glDeleteTextures(1, &tempTex);
        GL_CHECK();*/
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

    TextureDescriptor OpenGLTexture::descriptor() const
    {
        return { mTextureHandle, mType };
    }

    GLenum OpenGLTexture::target() const
    {
        switch (mType) {
        case TextureType_2D:
            return GL_TEXTURE_2D;
        case TextureType_2DMultiSample:
            return GL_TEXTURE_2D_MULTISAMPLE;
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