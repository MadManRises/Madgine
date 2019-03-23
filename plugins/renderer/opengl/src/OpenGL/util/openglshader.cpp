#include "../opengllib.h"

#include "openglshader.h"

namespace Engine {
	namespace Render {

		OpenGLShader::OpenGLShader(GLenum glType, ShaderType type) :
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