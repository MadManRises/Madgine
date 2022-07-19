#include "directx11lib.h"

#include "directx11textureloader.h"

#include "util/directx11texture.h"

#include "directx11rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11TextureLoader);

METATABLE_BEGIN(Engine::Render::DirectX11TextureLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX11TextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11TextureLoader::Resource, Engine::Render::TextureLoader::Resource)
METATABLE_END(Engine::Render::DirectX11TextureLoader::Resource)



namespace Engine {
namespace Render {

    DirectX11TextureLoader::DirectX11TextureLoader()
    {
    }

    bool DirectX11TextureLoader::loadImpl(DirectX11Texture &tex, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX11TextureLoader::unloadImpl(DirectX11Texture &tex)
    {
        tex.reset();
    }

    bool DirectX11TextureLoader::create(Texture &tex, TextureType type, DataFormat format /*, D3D11_BIND_FLAG bind*/)
    {
        UINT bind = D3D11_BIND_SHADER_RESOURCE;
        static_cast<DirectX11Texture &>(tex) = DirectX11Texture { type, format, bind };

        return true;
    }

    void DirectX11TextureLoader::setData(Texture &tex, Vector2i size, const ByteBuffer &data)
    {
        static_cast<DirectX11Texture &>(tex).setData(size, data);
    }

    void DirectX11TextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        static_cast<DirectX11Texture &>(tex).setSubData(offset, size, data);
    }

    Threading::TaskQueue *DirectX11TextureLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}
