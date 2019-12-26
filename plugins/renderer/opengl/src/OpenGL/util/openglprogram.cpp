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
    {
    }

    OpenGLProgram &OpenGLProgram::operator=(OpenGLProgram &&other)
    {
        std::swap(mHandle, other.mHandle);
    }

    bool OpenGLProgram::link(OpenGLShader *vertexShader, OpenGLShader *pixelShader, const std::vector<const char *> &attributeNames)
    {
        reset();

        if (vertexShader->mType != VertexShader || pixelShader->mType != PixelShader)
            std::terminate();

        mHandle = glCreateProgram();
        glAttachShader(mHandle, vertexShader->mHandle);
        glAttachShader(mHandle, pixelShader->mHandle);

        for (size_t i = 0; i < attributeNames.size(); ++i) {
            glBindAttribLocation(mHandle, i, attributeNames[i]);
            GL_CHECK();
        }

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
    }

    void OpenGLProgram::setUniform(const std::string &var, int value)
    {
        bind();
        GLint location = glGetUniformLocation(mHandle, var.c_str());
        glUniform1i(location, value);
        GL_CHECK();
    }

    void OpenGLProgram::setUniform(const std::string &var,
        const Matrix3 &value)
    {
        bind();
        GLint location = glGetUniformLocation(mHandle, var.c_str());
        Matrix3 transposed = value.Transpose();
        glUniformMatrix3fv(location, 1, GL_FALSE, transposed[0]);
        GL_CHECK();
    }

    void OpenGLProgram::setUniform(const std::string &var, const Matrix4 &value)
    {
        bind();
        GLint location = glGetUniformLocation(mHandle, var.c_str());
        Matrix4 transposed = value.Transpose();
        glUniformMatrix4fv(location, 1, GL_FALSE, transposed[0]);
        GL_CHECK();
    }

    void OpenGLProgram::setUniform(const std::string &var, const Vector3 &value)
    {
        bind();
        GLint location = glGetUniformLocation(mHandle, var.c_str());
        glUniform3fv(location, 1, value.ptr());
        GL_CHECK();
    }

}
}