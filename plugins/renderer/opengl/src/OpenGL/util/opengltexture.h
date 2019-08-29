#pragma once

namespace Engine {
	namespace Render {

		struct MADGINE_OPENGL_EXPORT OpenGLTexture {

			OpenGLTexture();
                    OpenGLTexture(const OpenGLTexture &) = delete;
                        OpenGLTexture(OpenGLTexture &&);
			~OpenGLTexture();

			void bind() const;

			void setData(GLsizei width, GLsizei height, void *data);

			GLuint handle() const;

			void setWrapMode(GLint mode);
			void setFilter(GLint filter);

		private:
			GLuint mHandle;
		};

	}
}