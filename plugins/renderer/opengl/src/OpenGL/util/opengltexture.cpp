#include "../opengllib.h"

#include "opengltexture.h"


namespace Engine {
	namespace Render {



		OpenGLTexture::OpenGLTexture()			
		{
			glGenTextures(1, &mHandle);
			glCheck();
		}

		OpenGLTexture::~OpenGLTexture()
		{
			glDeleteTextures(1, &mHandle);
			glCheck();
		}

		void OpenGLTexture::bind() const
		{
			glBindTexture(GL_TEXTURE_2D, mHandle);
			glCheck();
		}

		void OpenGLTexture::setData(GLsizei width, GLsizei height, void * data)
		{
			bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glCheck();
		}

		GLuint OpenGLTexture::handle()
		{
			return mHandle;
		}

		void OpenGLTexture::setWrapMode(GLint mode)
		{
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
			glCheck();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
			glCheck();
		}

		void OpenGLTexture::setFilter(GLint filter)
		{
			bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glCheck();
		}

	}
}