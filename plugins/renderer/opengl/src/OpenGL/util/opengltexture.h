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
        OpenGLTexture(OpenGLTexture &&) = default;
        ~OpenGLTexture();

        OpenGLTexture &operator=(OpenGLTexture &&);

        void reset();
        void bind() const;

        void setData(Vector2i size, const ByteBuffer &data);
        void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data);

        GLenum target() const;

        void setWrapMode(GLint mode);

        explicit operator bool() const;
        void setFilter(GLint filter);

    private:
        size_t mSamples = 1;
    };

}
}