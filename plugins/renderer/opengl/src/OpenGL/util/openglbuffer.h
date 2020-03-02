#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLBuffer {

        OpenGLBuffer() = default;
        OpenGLBuffer(GLenum target);
        OpenGLBuffer(const OpenGLBuffer &) = delete;
        OpenGLBuffer(OpenGLBuffer &&);
        ~OpenGLBuffer();

        OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;
        OpenGLBuffer &operator=(OpenGLBuffer &&);

        operator bool() const;

        void bind() const;

        void reset();
        void setData(const ByteBuffer &data);
        void setSubData(unsigned int offset, const ByteBuffer &data);

        GLuint handle();

    private:
        GLuint mHandle = 0;
        GLenum mTarget;
    };

}
}