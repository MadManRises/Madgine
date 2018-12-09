#include "opengllib.h"

#include "opengltexture.h"

#include "glad.h"

namespace Engine {
	namespace Render {



		OpenGLTexture::OpenGLTexture()			
		{
			glGenTextures(1, &mHandle);
		}

		OpenGLTexture::~OpenGLTexture()
		{
			glDeleteTextures(1, &mHandle);
		}

		void OpenGLTexture::bind() const
		{
			glBindTexture(GL_TEXTURE_2D, mHandle);
		}

		void OpenGLTexture::setData(size_t width, size_t height, void * data)
		{
			bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, data);
		}

		unsigned int OpenGLTexture::handle()
		{
			return mHandle;
		}

		void OpenGLTexture::setWrapMode(unsigned int mode)
		{
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
		}

		void OpenGLTexture::setFilter(unsigned int filter)
		{
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		}

	}
}