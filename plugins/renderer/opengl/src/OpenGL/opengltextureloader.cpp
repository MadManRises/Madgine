#include "opengllib.h"

#include "opengltextureloader.h"

#include "util/opengltexture.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "openglrendercontext.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLTextureLoader);

METATABLE_BEGIN(Engine::Render::OpenGLTextureLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLTextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLTextureLoader::Resource, Engine::Render::TextureLoader::Resource)
METATABLE_END(Engine::Render::OpenGLTextureLoader::Resource)

namespace Engine {
namespace Render {

    OpenGLTextureLoader::OpenGLTextureLoader()
    {
    }

    bool OpenGLTextureLoader::loadImpl(OpenGLTexture &tex, ResourceDataInfo &info)
    {
        throw 0;
    }

    void OpenGLTextureLoader::unloadImpl(OpenGLTexture &tex)
    {
        tex.reset();
    }

    bool OpenGLTextureLoader::create(Texture &tex, TextureType type, DataFormat format)
    {
        static_cast<OpenGLTexture &>(tex) = OpenGLTexture { type, format };

        return true;
    }

    void OpenGLTextureLoader::setData(Texture &tex, Vector2i size, const ByteBuffer &data)
    {
        static_cast<OpenGLTexture &>(tex) = { TextureType_2D, DataFormat::FORMAT_RGBA8 };
        static_cast<OpenGLTexture &>(tex).setData(size, data);
    }

    void OpenGLTextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        static_cast<OpenGLTexture &>(tex).setSubData(offset, size, data);
    }

    Threading::TaskQueue *OpenGLTextureLoader::loadingTaskQueue() const
    {
        return OpenGLRenderContext::renderQueue();
    }

}
}
