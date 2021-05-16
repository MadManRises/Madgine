#include "../opengllib.h"

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

    OpenGLVertexArray::OpenGLVertexArray(create_t)
    {
#if !OPENGL_ES
        glGenVertexArrays(1, &mHandle);
        GL_CHECK();
#endif
    }

    OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexArray &&other)
        :
#if !OPENGL_ES
        mHandle(std::exchange(other.mHandle, 0))
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
        std::swap(mHandle, other.mHandle);
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
        return mHandle != 0;
#else
        return mVBO != 0;
#endif
    }

    unsigned int OpenGLVertexArray::getCurrent()
    {
#if !OPENGL_ES
        GLint dummy;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &dummy);
        return dummy;
#else
        return reinterpret_cast<uintptr_t>(sCurrentBound);
#endif
    }

    void OpenGLVertexArray::reset()
    {
#if OPENGL_ES
        if (sCurrentBound == this)
            sCurrentBound = nullptr;
        mAttributes.clear();
        mVBO = 0;
        mEBO = 0;
#else
        if (mHandle) {
            glDeleteVertexArrays(1, &mHandle);
            GL_CHECK();
            mHandle = 0;
        }
#endif
    }

    void OpenGLVertexArray::bind()
    {
#if !OPENGL_ES
        glBindVertexArray(mHandle);
        GL_CHECK();
#else
        assert(sCurrentBound == nullptr);
        sCurrentBound = this;

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

    void OpenGLVertexArray::unbind()
    {
#if !OPENGL_ES
        glBindVertexArray(0);
        GL_CHECK();
#else
        assert(sCurrentBound == this);
        sCurrentBound = nullptr;
#endif
    }

    void OpenGLVertexArray::setVertexAttribute(unsigned int index, AttributeDescriptor attribute)
    {
        if (attribute) {

            if (attribute.mType == ATTRIBUTE_FLOAT) {
                glVertexAttribPointer(index, attribute.mArraySize, glType(attribute.mType), GL_FALSE, attribute.mStride, reinterpret_cast<void *>(attribute.mOffset));
            } else {
                glVertexAttribIPointer(index, attribute.mArraySize, glType(attribute.mType), attribute.mStride, reinterpret_cast<void *>(attribute.mOffset));
            }
            glEnableVertexAttribArray(index);

#if OPENGL_ES
            assert(sCurrentBound == this);
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
            assert(sCurrentBound == this);
            if (mAttributes.size() <= index) {
                if (mAttributes.size() < index)
                    LOG_WARNING("Non consecutive Vertex Attribute!");
                mAttributes.resize(index + 1);
            }
            mAttributes[index].mEnabled = false;
#endif
        }
    }

#if OPENGL_ES
    void OpenGLVertexArray::onBindVBO(GLuint buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mVBO = buffer;
    }

    void OpenGLVertexArray::onBindEBO(GLuint buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mEBO = buffer;
    }

    std::pair<unsigned int, unsigned int> OpenGLVertexArray::getCurrentBindings()
    {
        if (sCurrentBound) {
            return { sCurrentBound->mVBO, sCurrentBound->mEBO };
        } else {
            return { 0, 0 };
        }
    }

#else

    GLuint OpenGLVertexArray::handle()
    {
        return mHandle;
    }
#endif
}
}