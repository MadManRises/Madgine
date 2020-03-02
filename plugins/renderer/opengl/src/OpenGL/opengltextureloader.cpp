#include "opengllib.h"

#include "opengltextureloader.h"

#include "util/opengltexture.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLTextureLoader);

METATABLE_BEGIN(Engine::Render::OpenGLTextureLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLTextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLTextureLoader::ResourceType, Engine::Render::TextureLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLTextureLoader::ResourceType)

RegisterType(Engine::Render::OpenGLTextureLoader);

namespace Engine {
namespace Render {

    OpenGLTextureLoader::OpenGLTextureLoader()
    {
    }

    bool OpenGLTextureLoader::loadImpl(OpenGLTexture &tex, ResourceType *res)
    {
        throw 0;
    }

    void OpenGLTextureLoader::unloadImpl(OpenGLTexture &tex, ResourceType *res)
    {
        tex.reset();
    }

    bool OpenGLTextureLoader::create(Texture &tex, DataFormat format)
    {
        GLenum type;
        switch (format) {
        case FORMAT_UNSIGNED_BYTE:
            type = GL_UNSIGNED_BYTE;
            break;
        case FORMAT_FLOAT8:
            type = GL_FLOAT;
            break;
        default:
            std::terminate();
        }

        static_cast<OpenGLTexture &>(tex) = OpenGLTexture { type };

        return true;
    }

    void OpenGLTextureLoader::bind(const Texture &texture)
    {
        static_cast<const OpenGLTexture &>(texture).bind();
    }

    void OpenGLTextureLoader::bind(TextureHandle textureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        GL_CHECK();
    }

    void OpenGLTextureLoader::setData(Texture &tex, Vector2i size, const ByteBuffer &data)
    {
        static_cast<OpenGLTexture &>(tex).setData(size, data);
    }

    void OpenGLTextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        static_cast<OpenGLTexture &>(tex).setSubData(offset, size, data);
    }

    void OpenGLTextureLoader::setWrapMode(Texture &tex, WrapMode mode)
    {
        GLint glMode;
        switch (mode) {
        case WRAP_CLAMP_TO_EDGE:
            glMode = GL_CLAMP_TO_EDGE;
            break;
        default:
            std::terminate();
        }

        static_cast<OpenGLTexture &>(tex).setWrapMode(glMode);
    }

    void OpenGLTextureLoader::setMinMode(Texture &tex, MinMode mode)
    {
        GLint glMode;
        switch (mode) {
        case MIN_NEAREST:
            glMode = GL_LINEAR;
            break;
        default:
            std::terminate();
        }

        static_cast<OpenGLTexture &>(tex).setFilter(glMode);
    }

}
}
