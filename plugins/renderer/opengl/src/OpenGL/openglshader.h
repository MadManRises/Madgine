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
			OpenGLShader(int glType, ShaderType type);
			~OpenGLShader();

			unsigned int mHandle;
			ShaderType mType;
		};

	}
}