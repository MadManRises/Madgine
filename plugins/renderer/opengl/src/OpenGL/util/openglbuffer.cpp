#include "../opengllib.h"

#include "openglbuffer.h"

namespace Engine {
namespace Render {

    OpenGLBuffer::OpenGLBuffer()
    {
        glGenBuffers(1, &mHandle);
        glCheck();
    }

    OpenGLBuffer::OpenGLBuffer(dont_create_buffer_t)
    {
    }

    OpenGLBuffer::OpenGLBuffer(OpenGLBuffer &&other)
        : mHandle(std::exchange(other.mHandle, 0))
    {
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        if (mHandle) {
            glDeleteBuffers(1, &mHandle);
            glCheck();
        }
    }

    OpenGLBuffer &OpenGLBuffer::operator=(OpenGLBuffer &&other)
    {
        mHandle = std::exchange(other.mHandle, 0);
        return *this;
    }

    OpenGLBuffer::operator bool() const
    {
        return mHandle != 0;
    }

    void OpenGLBuffer::bind(GLenum target) const
    {
        glBindBuffer(target, mHandle);
        glCheck();
    }

    void OpenGLBuffer::setData(GLenum target, GLsizei size, const void *data)
    {
        bind(target);
        glBufferData(target, size, data, GL_STATIC_DRAW);
        glCheck();
    }

    GLuint OpenGLBuffer::handle()
    {
        return mHandle;
    }

}
}