#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLBuffer {

        OpenGLBuffer();
        OpenGLBuffer(dont_create_t);
        OpenGLBuffer(const OpenGLBuffer &) = delete;
        OpenGLBuffer(OpenGLBuffer &&);
        ~OpenGLBuffer();

		OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;
        OpenGLBuffer &operator=(OpenGLBuffer &&);

		operator bool() const;

        void bind(GLenum target) const;

		void reset();
        void setData(GLenum target, GLsizei size, const void *data);

        GLuint handle();

    private:
        GLuint mHandle = 0;
    };

}
}