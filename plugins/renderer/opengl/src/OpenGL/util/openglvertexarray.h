#pragma once

#include "Modules/generic/templates.h"

#include "Modules/math/vector4.h"

#include "Modules/render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_OPENGL_EXPORT OpenGLVertexArray {

        OpenGLVertexArray() = default;
        OpenGLVertexArray(const OpenGLVertexArray &) = delete;
        OpenGLVertexArray(OpenGLVertexArray &&);
        OpenGLVertexArray(create_t);
        ~OpenGLVertexArray();

        OpenGLVertexArray &operator=(OpenGLVertexArray &&other);

        static unsigned int getCurrent();

        void reset();

        void bind();
        void unbind();

#if OPENGL_ES
        static void onBindVBO(GLuint buffer);
        static void onBindEBO(GLuint buffer);

        static std::pair<unsigned int, unsigned int> getCurrentBindings();
#else
        GLuint handle();
#endif

        void enableVertexAttribute(unsigned int index, AttributeDescriptor attribute);

        void disableVertexAttribute(unsigned int index);

    private:
#if !OPENGL_ES
        GLuint mHandle = 0;
#else
        GLuint mVBO = 0;
        GLuint mEBO = 0;

        struct VertexArrayAttribute {
            bool mEnabled = false;
            AttributeDescriptor mAttribute;
        };

        std::vector<VertexArrayAttribute> mAttributes;
#endif
    };

}
}