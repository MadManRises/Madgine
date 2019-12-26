#pragma once

#include "Modules/math/vector2i.h"

#include "texture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTexture : Texture {

        OpenGLTexture(GLenum type);
        OpenGLTexture() = default;
        OpenGLTexture(const OpenGLTexture &) = delete;
        OpenGLTexture(OpenGLTexture &&);
        ~OpenGLTexture();

		OpenGLTexture &operator=(OpenGLTexture &&);

		void reset();

        void bind() const;

        void setData(Vector2i size, void *data);
        void setSubData(Vector2i offset, Vector2i size, void *data);

		void resize(Vector2i size);

        GLuint handle() const;

        void setWrapMode(GLint mode);
        void setFilter(GLint filter);

    private:
        GLenum mType;
        GLuint mHandle = 0;
        Vector2i mSize = { 0, 0 };
    };

}
}