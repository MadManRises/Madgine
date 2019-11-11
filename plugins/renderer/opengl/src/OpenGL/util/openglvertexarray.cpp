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
        default:
            std::terminate();
        }
    }

    OpenGLVertexArray::OpenGLVertexArray()
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
        mVBO(other.mVBO)
        , mEBO(other.mEBO)
        , mAttributes(std::move(other.mAttributes))
#endif
    {
    }

    OpenGLVertexArray::OpenGLVertexArray(dont_create_t)
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
        sCurrentBound = this;

        GL_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

        for (size_t i = 0; i < mAttributes.size(); ++i) {
            if (mAttributes[i].mEnabled) {
                glVertexAttribPointer(i, mAttributes[i].mAttribute.mArraySize, glType(mAttributes[i].mAttribute.mType, GL_FALSE, mAttributes[i].mAttribute.mStride, reinterpret_cast<void *>(mAttributes[i].mAttribute.mOffset));
                glEnableVertexAttribArray(i);
            } else {
                glDisableVertexAttribArray(i);
            }
        }
        static int maxAttribs = []() {
            int dummy;
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &dummy);
            return dummy;
        }();
        for (size_t i = mAttributes.size(); i < maxAttribs; ++i)
            glDisableVertexAttribArray(i);

#endif
    }

    void OpenGLVertexArray::enableVertexAttribute(unsigned int index, AttributeDescriptor attribute)
    {
        

        glVertexAttribPointer(index, attribute.mArraySize, glType(attribute.mType), GL_FALSE, attribute.mStride, reinterpret_cast<void *>(attribute.mOffset));
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
    }

    void OpenGLVertexArray::disableVertexAttribute(unsigned int index)
    {
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