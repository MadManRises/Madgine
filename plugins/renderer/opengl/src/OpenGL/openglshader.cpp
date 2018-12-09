#include "opengllib.h"

#include "openglshader.h"

#include "glad.h"

namespace Engine {
	namespace Render {

		OpenGLShader::OpenGLShader(int glType, ShaderType type) :
			mHandle(glCreateShader(glType)),
			mType(type)
		{			
		}

		OpenGLShader::~OpenGLShader()
		{
			glDeleteShader(mHandle);
		}

	}
}