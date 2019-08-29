#pragma once

namespace Engine {
namespace Render {

	constexpr struct dont_create_buffer_t {
    } dont_create_buffer;

    struct MADGINE_OPENGL_EXPORT OpenGLBuffer {

        OpenGLBuffer();
        OpenGLBuffer(dont_create_buffer_t);
        OpenGLBuffer(const OpenGLBuffer &) = delete;
        OpenGLBuffer(OpenGLBuffer &&);
        ~OpenGLBuffer();

		OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;
        OpenGLBuffer &operator=(OpenGLBuffer &&);

		operator bool() const;

        void bind(GLenum target) const;

        void setData(GLenum target, GLsizei size, const void *data);

        GLuint handle();

    private:
        GLuint mHandle = 0;
    };

}
}