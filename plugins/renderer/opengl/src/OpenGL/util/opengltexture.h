#pragma once

#include "Modules/math/vector2i.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTexture {

        OpenGLTexture();
        OpenGLTexture(const OpenGLTexture &) = delete;
        OpenGLTexture(OpenGLTexture &&);
        ~OpenGLTexture();

        void bind() const;

        void setData(Vector2i size, void *data);
        void setSubData(Vector2i offset, Vector2i size, void *data);

        GLuint handle() const;

        void setWrapMode(GLint mode);
        void setFilter(GLint filter);

    private:
        GLuint mHandle;
    };

}
}