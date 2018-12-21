#pragma once

namespace Engine {
	namespace Render {

		struct OpenGLTexture {

			OpenGLTexture();
			~OpenGLTexture();

			void bind() const;

			void setData(GLsizei width, GLsizei height, void *data);

			unsigned int handle();

			void setWrapMode(unsigned int mode);
			void setFilter(unsigned int filter);

		private:
			unsigned int mHandle;
		};

	}
}