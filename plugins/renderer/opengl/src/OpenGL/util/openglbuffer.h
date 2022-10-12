#pragma once

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLBuffer {

        OpenGLBuffer(GLenum target, const ByteBuffer &data = {});
        OpenGLBuffer(const OpenGLBuffer &) = delete;
        OpenGLBuffer(OpenGLBuffer &&);
        ~OpenGLBuffer();

        OpenGLBuffer &operator=(const OpenGLBuffer &) = delete;
        OpenGLBuffer &operator=(OpenGLBuffer &&);

        explicit operator bool() const;

        void bind() const;
        void bindVertex(size_t stride) const;

        void reset();
        void setData(const ByteBuffer &data);
        void resize(size_t size);
        WritableByteBuffer mapData(size_t offset = 0, size_t size = 0);
        template <typename T>
        auto mapData(size_t offset = 0, size_t size = 0)
        {
            return mapData(offset, size).cast<T>();
        }

        void setSubData(unsigned int offset, const ByteBuffer &data);

        GLuint handle() const;

    private:
        GLuint mHandle = 0;
        GLenum mTarget;
        size_t mSize = 0;
    };

}
}