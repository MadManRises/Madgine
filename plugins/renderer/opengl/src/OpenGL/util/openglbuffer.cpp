#include "../opengllib.h"

#include "openglbuffer.h"

#if OPENGL_ES
#    include "openglvertexarray.h"
#endif

namespace Engine {
namespace Render {

    OpenGLBuffer::OpenGLBuffer()
    {
        glGenBuffers(1, &mHandle);
        GL_CHECK();
    }

    OpenGLBuffer::OpenGLBuffer(dont_create_t)
    {
    }

    OpenGLBuffer::OpenGLBuffer(OpenGLBuffer &&other)
        : mHandle(std::exchange(other.mHandle, 0))
    {
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        reset();
    }

    OpenGLBuffer &OpenGLBuffer::operator=(OpenGLBuffer &&other)
    {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    OpenGLBuffer::operator bool() const
    {
        return mHandle != 0;
    }

    void OpenGLBuffer::bind(GLenum target) const
    {
        glBindBuffer(target, mHandle);
        GL_LOG("Bind Buffer " << target << " -> " << mHandle);
        GL_CHECK();
#if OPENGL_ES
        if (target == GL_ELEMENT_ARRAY_BUFFER)
            OpenGLVertexArray::onBindEBO(mHandle);
        if (target == GL_ARRAY_BUFFER)
            OpenGLVertexArray::onBindVBO(mHandle);
#endif
    }

    void OpenGLBuffer::reset()
    {
        if (mHandle) {
            glDeleteBuffers(1, &mHandle);
            GL_CHECK();
            mHandle = 0;
        }
    }

    void OpenGLBuffer::setData(GLenum target, GLsizei size, const void *data)
    {
        bind(target);
        glBufferData(target, size, data, GL_STATIC_DRAW);
        GL_CHECK();
    }

    GLuint OpenGLBuffer::handle()
    {
        return mHandle;
    }

}
}