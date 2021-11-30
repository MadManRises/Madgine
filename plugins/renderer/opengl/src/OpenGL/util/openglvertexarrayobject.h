#pragma once

#include "render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_OPENGL_EXPORT OpenGLVertexArrayObject {

        OpenGLVertexArrayObject() = default;
        OpenGLVertexArrayObject(const OpenGLVertexArrayObject &) = delete;
        OpenGLVertexArrayObject(OpenGLVertexArrayObject &&);
        OpenGLVertexArrayObject(OpenGLBuffer &vertex, OpenGLBuffer &index, const std::array<AttributeDescriptor, 7> &attributes, const OpenGLBuffer &instanceDataBuffer, size_t instanceDataSize);
        ~OpenGLVertexArrayObject();

        OpenGLVertexArrayObject &operator=(OpenGLVertexArrayObject &&other);

		explicit operator bool() const;

        void reset();

        void bind() const;
        void unbind();

#if !OPENGL_ES || OPENGL_ES >= 300
        GLuint handle();
#endif

        void setVertexAttribute(unsigned int index, AttributeDescriptor attribute);

    private:
#if !OPENGL_ES || OPENGL_ES >= 300
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