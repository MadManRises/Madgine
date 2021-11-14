#include "../opengllib.h"

#include "openglbuffer.h"
#include "openglvertexarray.h"

namespace Engine {
namespace Render {

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
        : mAttributes(std::exchange(other.mAttributes, nullptr))
        , mVertexBuffer(std::exchange(other.mVertexBuffer, nullptr))
        , mIndexBuffer(std::exchange(other.mIndexBuffer, nullptr))
    {
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        reset();
    }

    OpenGLVertexArray &OpenGLVertexArray::operator=(OpenGLVertexArray &&other)
    {
        std::swap(mAttributes, other.mAttributes);
        std::swap(mVertexBuffer, other.mVertexBuffer);
        std::swap(mIndexBuffer, other.mIndexBuffer);
        return *this;
    }

    OpenGLVertexArray::operator bool() const
    {
        return mAttributes;
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