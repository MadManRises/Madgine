#include "../opengllib.h"

#include "openglbuffer.h"
#include "openglvertexarrayobject.h"

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

    OpenGLVertexArrayObject::OpenGLVertexArrayObject(OpenGLBuffer &vertex, OpenGLBuffer &index, const std::array<AttributeDescriptor, 7> &attributes, OpenGLBuffer &instanceDataBuffer, size_t instanceDataSize)
#if OPENGL_ES && OPENGL_ES < 300
        : mVBO(vertex.handle())
        , mEBO(index.handle())
#endif
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        glGenVertexArrays(1, &mHandle);
        GL_CHECK();
        glBindVertexArray(mHandle);
        GL_CHECK();
        vertex.bind();
        index.bind();
#endif

        for (size_t i = 0; i < attributes.size(); ++i) {
            setVertexAttribute(i, attributes[i]);
        }

#if !OPENGL_ES || OPENGL_ES >= 300
        if (instanceDataSize > 0) {
            assert(instanceDataSize % 16 == 0);

            instanceDataBuffer.bind();
            for (size_t i = 0; i < instanceDataSize / 16; ++i) {
                glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, instanceDataSize, reinterpret_cast<void *>(i * sizeof(float[4])));
                GL_CHECK();
                glVertexAttribDivisor(7 + i, 1);
                GL_CHECK();
                glEnableVertexAttribArray(7 + i);
                GL_CHECK();
            }
        }

        unbind();
#endif
    }

    OpenGLVertexArrayObject::OpenGLVertexArrayObject(OpenGLVertexArrayObject &&other)
        :
#if !OPENGL_ES || OPENGL_ES >= 300
        mHandle(std::exchange(other.mHandle, 0))
#else
        mVBO(std::exchange(other.mVBO, 0))
        , mEBO(std::exchange(other.mEBO, 0))
        , mAttributes(std::move(other.mAttributes))
#endif
    {
    }

    OpenGLVertexArrayObject::~OpenGLVertexArrayObject()
    {
        reset();
    }

    OpenGLVertexArrayObject &OpenGLVertexArrayObject::operator=(OpenGLVertexArrayObject &&other)
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        std::swap(mHandle, other.mHandle);
#else
        mAttributes = std::move(other.mAttributes);
        mVBO = std::exchange(other.mVBO, 0);
        mEBO = std::exchange(other.mEBO, 0);
#endif
        return *this;
    }

    OpenGLVertexArrayObject::operator bool() const
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        return mHandle != 0;
#else
        return mVBO != 0;
#endif
    }

    void OpenGLVertexArrayObject::reset()
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        if (mHandle) {
            glDeleteVertexArrays(1, &mHandle);
            GL_CHECK();
            mHandle = 0;
        }
#else
        mAttributes.clear();
        mVBO = 0;
        mEBO = 0;
#endif
    }

    void OpenGLVertexArrayObject::bind()
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        glBindVertexArray(mHandle);
        GL_CHECK();
#else
        GL_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

        for (size_t i = 0; i < mAttributes.size(); ++i) {
            if (mAttributes[i].mEnabled) {
                if (mAttributes[i].mAttribute.mType == ATTRIBUTE_FLOAT) {
                    glVertexAttribPointer(i, mAttributes[i].mAttribute.mArraySize, glType(mAttributes[i].mAttribute.mType), GL_FALSE, mAttributes[i].mAttribute.mStride, reinterpret_cast<void *>(mAttributes[i].mAttribute.mOffset));
                    GL_LOG("Use " << i << " as vec" << mAttributes[i].mAttribute.mArraySize);
                } else {
                    glVertexAttribIPointer(i, mAttributes[i].mAttribute.mArraySize, glType(mAttributes[i].mAttribute.mType), mAttributes[i].mAttribute.mStride, reinterpret_cast<void *>(mAttributes[i].mAttribute.mOffset));
                    GL_LOG("Use " << i << " as ivec" << mAttributes[i].mAttribute.mArraySize);
                }
                glEnableVertexAttribArray(i);
            } else {
                glDisableVertexAttribArray(i);
                GL_LOG("Dont use " << i);
            }
        }
        static int maxAttribs = []() {
            int dummy;
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &dummy);
            return dummy;
        }();
        for (size_t i = mAttributes.size(); i < maxAttribs; ++i) {
            glDisableVertexAttribArray(i);
            GL_LOG("Dont use " << i);
        }

#endif
    }

    void OpenGLVertexArrayObject::unbind()
    {
#if !OPENGL_ES || OPENGL_ES >= 300
        glBindVertexArray(0);
        GL_CHECK();
#endif
    }

    void OpenGLVertexArrayObject::setVertexAttribute(unsigned int index, AttributeDescriptor attribute)
    {
        if (attribute) {

            if (attribute.mType == ATTRIBUTE_FLOAT) {
                glVertexAttribPointer(index, attribute.mArraySize, glType(attribute.mType), GL_FALSE, attribute.mStride, reinterpret_cast<void *>(attribute.mOffset));
            } else {
                glVertexAttribIPointer(index, attribute.mArraySize, glType(attribute.mType), attribute.mStride, reinterpret_cast<void *>(attribute.mOffset));
            }
            GL_CHECK();
            glEnableVertexAttribArray(index);

#if OPENGL_ES
            if (mAttributes.size() <= index) {
                if (mAttributes.size() < index)
                    LOG_WARNING("Non consecutive Vertex Attribute!");
                mAttributes.resize(index + 1);
            }
            mAttributes[index] = { true, attribute };
#endif
        } else {
            glDisableVertexAttribArray(index);

#if OPENGL_ES
            if (mAttributes.size() <= index) {
                if (mAttributes.size() < index)
                    LOG_WARNING("Non consecutive Vertex Attribute!");
                mAttributes.resize(index + 1);
            }
            mAttributes[index].mEnabled = false;
#endif
        }
        GL_CHECK();
    }

#if !OPENGL_ES || OPENGL_ES >= 300

    GLuint OpenGLVertexArrayObject::handle()
    {
        return mHandle;
    }
#endif
}
}