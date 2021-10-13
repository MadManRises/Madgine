#include "../opengllib.h"

#include "openglbuffer.h"
#include "openglvertexarray.h"

namespace Engine {
namespace Render {

#if OPENGL_ES
    static thread_local OpenGLVertexArray *sCurrentBound = nullptr;
#endif

    static GLenum glType(AttributeType type)
    {
        switch (type) {
        case ATTRIBUTE_FLOAT:
            return GL_FLOAT;
        case ATTRIBUTE_INT:
            return GL_INT;
        default:
            std::terminate();
        }
    }

    OpenGLVertexArray::OpenGLVertexArray(OpenGLBuffer &vertex, OpenGLBuffer &index, std::array<AttributeDescriptor, 7> (*attributes)())
        : mAttributes(attributes)
        , mVertexBuffer(&vertex)
        , mIndexBuffer(&index)
    {
    }

    OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray &&other)
        :
#if !OPENGL_ES
        mAttributes(std::exchange(other.mAttributes, nullptr))
        , mVertexBuffer(std::exchange(other.mVertexBuffer, nullptr))
        , mIndexBuffer(std::exchange(other.mIndexBuffer, nullptr))
#else
        mVBO(std::exchange(other.mVBO, 0))
        , mEBO(std::exchange(other.mEBO, 0))
        , mAttributes(std::move(other.mAttributes))
#endif
    {
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        reset();
    }

    OpenGLVertexArray &OpenGLVertexArray::operator=(OpenGLVertexArray &&other)
    {
#if !OPENGL_ES
        std::swap(mAttributes, other.mAttributes);
        std::swap(mVertexBuffer, other.mVertexBuffer);
        std::swap(mIndexBuffer, other.mIndexBuffer);
#else
        if (sCurrentBound == &other)
            sCurrentBound = this;
        mAttributes = std::move(other.mAttributes);
        mVBO = std::exchange(other.mVBO, 0);
        mEBO = std::exchange(other.mEBO, 0);
#endif
        return *this;
    }

    OpenGLVertexArray::operator bool() const
    {
#if !OPENGL_ES
        return mAttributes;
#else
        return mVBO != 0;
#endif
    }

    void OpenGLVertexArray::reset()
    {
        mAttributes = nullptr;
    }

    void OpenGLVertexArray::bind(OpenGLProgram *program, OpenGLBuffer &instanceDataBuffer, size_t instanceDataSize)
    {
        OpenGLVertexArrayObject &instance = mInstances[program];
        if (!instance)
            instance = { *mVertexBuffer, *mIndexBuffer, mAttributes(), instanceDataBuffer, instanceDataSize };
        instance.bind();
    }

     void OpenGLVertexArray::unbind(OpenGLProgram *program)
    {
         mInstances.at(program).unbind();
    }

}
}