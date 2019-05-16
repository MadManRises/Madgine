#include "opengllib.h"

#include "openglshaderprogram.h"
#include "util/openglshader.h"


#include "Interfaces/math/matrix4.h"

namespace Engine {
	namespace Render {

		OpenGLShaderProgram::~OpenGLShaderProgram()
		{
			reset();
		}

		bool OpenGLShaderProgram::link(OpenGLShader * vertexShader, OpenGLShader * pixelShader)
		{
			reset();

			if (vertexShader->mType != VertexShader || pixelShader->mType != PixelShader)
				throw 0;

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

			return success;
		}

		void OpenGLShaderProgram::reset()
		{
			if (mHandle)
			{
				glDeleteProgram(mHandle);
				mHandle = 0;
			}
		}

		void OpenGLShaderProgram::bind()
		{
			assert(mHandle);
			glUseProgram(mHandle);
		}

		void OpenGLShaderProgram::setUniform(const std::string & var, int value)
		{
			bind();
			GLint location = glGetUniformLocation(mHandle, var.c_str());
			glUniform1i(location, value);
        }

        void OpenGLShaderProgram::setUniform(const std::string &var,
                                                const Matrix3 &value) 
		{
            bind();
            GLint location = glGetUniformLocation(mHandle, var.c_str());
            glUniformMatrix3fv(location, 1, GL_TRUE, value[0]);
		}

		void OpenGLShaderProgram::setUniform(const std::string & var, const Matrix4 & value)
		{
			bind();
			GLint location = glGetUniformLocation(mHandle, var.c_str());
			glUniformMatrix4fv(location, 1, GL_TRUE, value[0]);
		}

		void OpenGLShaderProgram::setUniform(const std::string & var, const Vector3 & value)
		{
			bind();
			GLint location = glGetUniformLocation(mHandle, var.c_str());
			glUniform3fv(location, 1, value.ptr());
		}


	}
}