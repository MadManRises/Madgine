#pragma once

namespace Engine {
	namespace Render {

		struct OpenGLTexture {

			OpenGLTexture();
			~OpenGLTexture();

			void bind() const;

			void setData(size_t width, size_t height, void *data);

			unsigned int handle();

			void setWrapMode(unsigned int mode);
			void setFilter(unsigned int filter);

		private:
			unsigned int mHandle;
		};

	}
}