#include "../opengllib.h"

#include "openglvertexarray.h"

namespace Engine {
namespace Render {

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glGenVertexArrays(1, &mHandle);
        glCheck();
    }

    OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray &&other)
        : mHandle(std::exchange(other.mHandle, 0))
    {
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        if (mHandle) {
            glDeleteVertexArrays(1, &mHandle);
            glCheck();
        }
    }

    void OpenGLVertexArray::bind() const
    {
        glBindVertexArray(mHandle);        
        glCheck();
    }

    GLuint OpenGLVertexArray::handle()
    {
        return mHandle;
    }

}
}