#include "../opengllib.h"

#include "openglbuffer.h"

#if OPENGL_ES
#    include "openglvertexarray.h"
#endif

namespace Engine {
namespace Render {

    OpenGLBuffer::OpenGLBuffer(GLenum target)
        : mTarget(target)
    {
        glGenBuffers(1, &mHandle);
        GL_CHECK();
    }

    OpenGLBuffer::OpenGLBuffer(OpenGLBuffer &&other)
        : mHandle(std::exchange(other.mHandle, 0))
        , mTarget(std::exchange(other.mTarget, 0))
    {
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        reset();
    }

    OpenGLBuffer &OpenGLBuffer::operator=(OpenGLBuffer &&other)
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mTarget, other.mTarget);
        return *this;
    }

    OpenGLBuffer::operator bool() const
    {
        return mHandle != 0;
    }

    void OpenGLBuffer::bind() const
    {
        glBindBuffer(mTarget, mHandle);
        GL_LOG("Bind Buffer " << mTarget << " -> " << mHandle);
        GL_CHECK();
#if OPENGL_ES
        if (mTarget == GL_ELEMENT_ARRAY_BUFFER)
            OpenGLVertexArray::onBindEBO(mHandle);
        if (mTarget == GL_ARRAY_BUFFER)
            OpenGLVertexArray::onBindVBO(mHandle);
#endif
    }

    void OpenGLBuffer::reset()
    {
        if (mHandle) {
            glDeleteBuffers(1, &mHandle);
            GL_CHECK();
            mHandle = 0;
            mTarget = 0;
        }
    }

    void OpenGLBuffer::setData(const void *data, GLsizei size)
    {
        bind();
        glBufferData(mTarget, size, data, GL_STATIC_DRAW);
        GL_CHECK();
        GL_LOG("Buffer-Data: " << size);
    }

    void OpenGLBuffer::setSubData(const void *data, unsigned int offset, GLsizei size)
    {
        bind();
        glBufferSubData(mTarget, offset, size, data);
        GL_LOG("Buffer-SubData: " << offset << ", " << size);
        GL_CHECK();
    }

    GLuint OpenGLBuffer::handle()
    {
        return mHandle;
    }

}
}