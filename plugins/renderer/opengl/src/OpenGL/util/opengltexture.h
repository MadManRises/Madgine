#pragma once

#include "Meta/math/vector2i.h"

#include "Madgine/render/texture.h"

#include "Madgine/render/texturedescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTexture : Texture {

        OpenGLTexture(TextureType type, DataFormat format, size_t samples = 1);
        OpenGLTexture() = default;
        OpenGLTexture(const OpenGLTexture &) = delete;
        OpenGLTexture(OpenGLTexture &&);
        ~OpenGLTexture();

        OpenGLTexture &operator=(OpenGLTexture &&);

        void reset();
        void bind() const;

        void setData(Vector2i size, const ByteBuffer &data);
        void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data);

        TextureDescriptor descriptor() const;
        GLenum target() const;

        void resize(Vector2i size);

        void setWrapMode(GLint mode);

        explicit operator bool() const;
        void setFilter(GLint filter);

    private:
        DataFormat mFormat;
        TextureType mType;
        Vector2i mSize = { 0, 0 };
        size_t mSamples = 1;
    };

}
}