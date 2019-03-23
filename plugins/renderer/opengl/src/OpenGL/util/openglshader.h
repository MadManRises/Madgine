#pragma once

namespace Engine {
	namespace Render {

		enum ShaderType 
		{
			VertexShader,
			PixelShader
		};

		struct OpenGLShader 
		{
			OpenGLShader(GLenum glType, ShaderType type);
			~OpenGLShader();

			GLuint mHandle;
			ShaderType mType;
		};

	}
}