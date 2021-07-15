#include "directx12lib.h"

#include "directx12textureloader.h"

#include "util/directx12texture.h"

#include "directx12rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX12TextureLoader);

METATABLE_BEGIN(Engine::Render::DirectX12TextureLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::DirectX12TextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12TextureLoader::ResourceType, Engine::Render::TextureLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX12TextureLoader::ResourceType)



namespace Engine {
namespace Render {

    DirectX12TextureLoader::DirectX12TextureLoader()
    {
    }

    bool DirectX12TextureLoader::loadImpl(DirectX12Texture &tex, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX12TextureLoader::unloadImpl(DirectX12Texture &tex, ResourceDataInfo &info)
    {
        tex.reset();
    }

    bool DirectX12TextureLoader::create(Texture &tex, DataFormat format/*, D3D12_BIND_FLAG bind*/)
    {
        static_cast<DirectX12Texture &>(tex) = DirectX12Texture { Texture2D, format };

        return true;
    }

    void DirectX12TextureLoader::setData(Texture &tex, Vector2i size, const ByteBuffer &data)
    {
        static_cast<DirectX12Texture &>(tex).setData(size, data);
    }

    void DirectX12TextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        static_cast<DirectX12Texture &>(tex).setSubData(offset, size, data);
    }

}
}
