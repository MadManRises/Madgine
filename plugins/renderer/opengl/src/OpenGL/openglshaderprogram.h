#pragma once

namespace Engine {
	namespace Render {

		struct OpenGLShaderProgram {			
			
			~OpenGLShaderProgram();

			bool link(OpenGLShader *vertexShader, OpenGLShader *pixelShader);

			void reset();

			void bind();

			void setUniform(const std::string &var, int value);
			void setUniform(const std::string &var, const Matrix4 &value);
			void setUniform(const std::string &var, const Vector3 &value);

		private:
			unsigned int mHandle = 0;
		};

	}
}