#include "../opengllib.h"

#include "openglprogram.h"
#include "openglshader.h"

#include "Modules/math/matrix4.h"

namespace Engine {
namespace Render {

    OpenGLProgram::~OpenGLProgram()
    {
        reset();
    }

    OpenGLProgram::OpenGLProgram(OpenGLProgram &&other)
        : mHandle(std::exchange(other.mHandle, 0))
        , mUniformBuffers(std::move(other.mUniformBuffers))
    {
    }

    OpenGLProgram &OpenGLProgram::operator=(OpenGLProgram &&other)
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mUniformBuffers, other.mUniformBuffers);
        return *this;
    }

    bool OpenGLProgram::link(OpenGLShader *vertexShader, OpenGLShader *pixelShader)
    {
        reset();

        if (vertexShader->mType != VertexShader || pixelShader->mType != PixelShader)
            std::terminate();

        mHandle = glCreateProgram();
        glAttachShader(mHandle, vertexShader->mHandle);
        glAttachShader(mHandle, pixelShader->mHandle);

        glLinkProgram(mHandle);
        // check for linking errors
        GLint success;
        char infoLog[512];
        glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mHandle, 512, NULL, infoLog);
            LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED");
            LOG_ERROR(infoLog);
        }

#if OPENGL_ES
        GLuint perApplicationIndex = glGetUniformBlockIndex(mHandle, "PerApplication");
        if (perApplicationIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(mHandle, perApplicationIndex, 0);
        }

        GLuint perFrameIndex = glGetUniformBlockIndex(mHandle, "PerFrame");
        if (perFrameIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(mHandle, perFrameIndex, 1);
        }

        GLuint perObjectIndex = glGetUniformBlockIndex(mHandle, "PerObject");
        if (perObjectIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(mHandle, perObjectIndex, 2);
        }
#endif

        return success;
    }

    void OpenGLProgram::reset()
    {
        if (mHandle) {
            glDeleteProgram(mHandle);
            mHandle = 0;
        }
    }

    void OpenGLProgram::bind()
    {
        assert(mHandle);
        glUseProgram(mHandle);

        for (size_t i = 0; i < mUniformBuffers.size(); ++i) {
            glBindBufferBase(GL_UNIFORM_BUFFER, i, mUniformBuffers[i].handle());
        }
    }

    void OpenGLProgram::setParameters(const void *data, size_t size, size_t index)
    {
        if (mUniformBuffers.size() <= index)
            mUniformBuffers.resize(index + 1);

        if (!mUniformBuffers[index]) {
            mUniformBuffers[index] = GL_UNIFORM_BUFFER;
        }

        mUniformBuffers[index].setData(size, data);
    }

}
}