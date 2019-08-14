#include "../opengllib.h"

#include "openglbuffer.h"


namespace Engine {
	namespace Render {



		OpenGLBuffer::OpenGLBuffer()
		{
			glGenBuffers(1, &mHandle);
			glCheck();
		}

		OpenGLBuffer::~OpenGLBuffer()
		{
			glDeleteBuffers(1, &mHandle);
			glCheck();
		}

		void OpenGLBuffer::bind(GLenum target) const
		{
			glBindBuffer(target, mHandle);
			glCheck();
		}

		void OpenGLBuffer::setData(GLenum target, GLsizei size, const void * data)
		{
			bind(target);
			glBufferData(target, size, data, GL_STATIC_DRAW);
			glCheck();
		}

		GLuint OpenGLBuffer::handle()
		{
			return mHandle;
		}


	}
}