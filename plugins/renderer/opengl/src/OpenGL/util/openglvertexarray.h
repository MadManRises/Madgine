#pragma once

#include "Modules/generic/templates.h"

#include "Modules/math/vector4.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLVertexArray {

        OpenGLVertexArray();
        OpenGLVertexArray(const OpenGLVertexArray &) = delete;
        OpenGLVertexArray(OpenGLVertexArray &&);
        OpenGLVertexArray(dont_create_t);
        ~OpenGLVertexArray();

		OpenGLVertexArray &operator=(OpenGLVertexArray &&other);

		static unsigned int getCurrent();

		void reset();

        void bind();

#if OPENGL_ES
        static void onBindVBO(GLuint buffer);
        static void onBindEBO(GLuint buffer);

		static std::pair<unsigned int, unsigned int> getCurrentBindings();
#else
        GLuint handle();
#endif

        template <typename T, typename M>
        void enableVertexAttribute(unsigned int index, M T::*m)
        {
            GLenum type;
            GLint size;
            if constexpr (std::is_same_v<M, Vector2>) {
                type = GL_FLOAT;
                size = 2;
            } else if constexpr (std::is_same_v<M, Vector3>) {
                type = GL_FLOAT;
                size = 3;
            } else if constexpr (std::is_same_v<M, Vector4>) {
                type = GL_FLOAT;
                size = 4;
            } else {
                static_assert(dependent_bool<M, false>::value, "Unsupported Attribute type!");
            }
            enableVertexAttribute(index, size, type, sizeof(T), static_cast<GLsizei>(reinterpret_cast<size_t>(&(static_cast<T *>(nullptr)->*m))));
        }
        void enableVertexAttribute(unsigned int index, GLint size, GLenum type, GLsizei stride, GLsizei offset);

        void disableVertexAttribute(unsigned int index);

    private:
#if !OPENGL_ES
        GLuint mHandle = 0;
#else
        GLuint mVBO = 0;
        GLuint mEBO = 0;

        struct VertexArrayAttribute {
            bool mEnabled = false;
            GLint mSize;
            GLenum mType;
            //GLboolean normalized,
            GLsizei mStride;
            GLsizei mOffset;
        };

        std::vector<VertexArrayAttribute> mAttributes;
#endif
    };

}
}