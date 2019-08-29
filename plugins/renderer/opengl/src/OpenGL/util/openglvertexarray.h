#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLVertexArray {

        OpenGLVertexArray();
        OpenGLVertexArray(const OpenGLVertexArray &) = delete;
        OpenGLVertexArray(OpenGLVertexArray &&);
        ~OpenGLVertexArray();

        void bind() const;

        GLuint handle();

    private:
        GLuint mHandle;
    };

}
}