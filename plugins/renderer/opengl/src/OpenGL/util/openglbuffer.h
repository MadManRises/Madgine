#pragma once

namespace Engine {
	namespace Render {

		struct OpenGLBuffer {

			OpenGLBuffer();
			~OpenGLBuffer();

			void bind(GLenum target) const;

			void setData(GLenum target, GLsizei size, const void *data);

			GLuint handle();

		private:
			GLuint mHandle;
		};

	}
}