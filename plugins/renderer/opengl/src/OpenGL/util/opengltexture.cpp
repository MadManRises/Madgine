#include "../opengllib.h"

#include "opengltexture.h"

#include "Generic/areaview.h"
#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLTexture::OpenGLTexture(GLenum type)
        : mType(type)
    {
        GLuint temp;
        glGenTextures(1, &temp);
        mTextureHandle = temp;
        GL_CHECK();
        setFilter(GL_LINEAR);
        bind();
#if !OPENGL_ES
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        GL_CHECK();
#endif
    }

    OpenGLTexture::OpenGLTexture(OpenGLTexture &&other)
        : mType(other.mType)
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
        glBindTexture(GL_TEXTURE_2D, mTextureHandle);
        GL_CHECK();
    }

    void OpenGLTexture::setData(Vector2i size, const ByteBuffer &data)
    {
        const void *ptr = data.mData;
        std::vector<unsigned char> flippedData;
        if (ptr) {
            size_t byteWidth;
            switch (mType) {
            case GL_UNSIGNED_BYTE:
                byteWidth = 4;
                break;
            default:
                throw 0;
            }
            flippedData.reserve(size.x * size.y * byteWidth);
            AreaView<const unsigned char, 2> view { static_cast<const unsigned char *>(data.mData), { size.x * byteWidth, static_cast<size_t>(size.y) } };
            view.flip(1);
            std::copy(view.begin(), view.end(), std::back_inserter(flippedData));
            ptr = flippedData.data();
        }
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, mType, ptr);
        GL_CHECK();
        mSize = size;
    }

    void OpenGLTexture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        std::vector<unsigned char> flippedData;
        size_t byteWidth;
        switch (mType) {
        case GL_UNSIGNED_BYTE:
            byteWidth = 4;
            break;
        default:
            throw 0;
        }
        flippedData.reserve(size.x * size.y * byteWidth);
        AreaView<const unsigned char, 2> view { static_cast<const unsigned char *>(data.mData), { size.x * byteWidth, static_cast<size_t>(size.y) } };
        view.flip(1);
        std::copy(view.begin(), view.end(), std::back_inserter(flippedData));
        bind();
        glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, mSize.y - offset.y - size.y, size.x, size.y, GL_RGBA, mType, flippedData.data());
        GL_CHECK();
    }

    void OpenGLTexture::resize(Vector2i size)
    {
        throw "TODO";
        /*Vector2i commonSize = min(size, mSize);
        GLuint tempTex;
        glGenTextures(1, &tempTex);
        GL_CHECK();
        std::swap(tempTex, mHandle);

        setData(size, nullptr);

        glCopyImageSubData(tempTex, GL_TEXTURE_2D, 0, 0, 0, 0, mHandle, GL_TEXTURE_2D, 0, 0, 0, 0, commonSize.x, commonSize.y, 1);
        GL_CHECK();

        glDeleteTextures(1, &tempTex);
        GL_CHECK();*/
    }

    void OpenGLTexture::setWrapMode(GLint mode)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        GL_CHECK();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        GL_CHECK();
    }

    void OpenGLTexture::setFilter(GLint filter)
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        GL_CHECK();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        GL_CHECK();
    }

}
}