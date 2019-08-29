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
                    OpenGLShader(const OpenGLShader &) = delete;
                        OpenGLShader(OpenGLShader &&);
			~OpenGLShader();

			GLuint mHandle;
			ShaderType mType;
		};

	}
}