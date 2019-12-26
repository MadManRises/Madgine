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
			OpenGLShader() = default;
			OpenGLShader(GLenum glType, ShaderType type);
                    OpenGLShader(const OpenGLShader &) = delete;
                        OpenGLShader(OpenGLShader &&);
			~OpenGLShader();

			OpenGLShader &operator=(OpenGLShader &&);

			void reset();

			GLuint mHandle = 0;
			ShaderType mType;
		};

	}
}