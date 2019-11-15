#include "opengllib.h"

#include "opengltextureloader.h"

#include "util/opengltexture.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLTextureLoader);

using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Render::Texture, Engine::Resources::ThreadLocalResource>;

METATABLE_BEGIN_BASE(Engine::Render::OpenGLTextureLoader, LoaderImpl)
METATABLE_END(Engine::Render::OpenGLTextureLoader)

RegisterType(Engine::Render::OpenGLTextureLoader);

namespace Engine {
namespace Render {

    OpenGLTextureLoader::OpenGLTextureLoader()
    {
    }

    std::shared_ptr<Texture> OpenGLTextureLoader::loadImpl(ResourceType *res)
    {
        throw 0;
    }

    std::shared_ptr<Texture> OpenGLTextureLoader::create(DataFormat format)
    {
        GLenum type;
        switch (format) {
        case FORMAT_UNSIGNED_BYTE:
            type = GL_UNSIGNED_BYTE;
            break;
        default:
            std::terminate();
		}

        std::shared_ptr<OpenGLTexture> texture = std::make_shared<OpenGLTexture>(type);

        
        return texture;
    }

    void OpenGLTextureLoader::bind(const Texture &texture)
    {
        static_cast<const OpenGLTexture &>(texture).bind();
    }

    void OpenGLTextureLoader::bind(unsigned int textureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        GL_CHECK();
    }

    void OpenGLTextureLoader::setData(Texture &tex, Vector2i size, void *data)
    {
        static_cast<OpenGLTexture &>(tex).setData(size, data);
    }

    void OpenGLTextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, void *data)
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



}
}
