#pragma once

namespace Engine {
	namespace Render {

		struct OpenGLTexture {

			OpenGLTexture();
			~OpenGLTexture();

			void bind() const;

			void setData(GLsizei width, GLsizei height, void *data);

			GLuint handle();

			void setWrapMode(GLint mode);
			void setFilter(GLint filter);

		private:
			GLuint mHandle;
		};

	}
}