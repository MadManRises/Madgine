#pragma once

#include "Madgine/resources/resourceloader.h"

#include "textureloader.h"

#include "util/directx12texture.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12TextureLoader : Resources::VirtualResourceLoaderImpl<DirectX12TextureLoader, DirectX12Texture, TextureLoader> {
        DirectX12TextureLoader();

        bool loadImpl(DirectX12Texture &tex, ResourceDataInfo &info);
        void unloadImpl(DirectX12Texture &tex, ResourceDataInfo &info);
        bool create(Texture &texture, DataFormat format/*, D3D12_BIND_FLAG bind*/) override;

		virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) override;
	};
}
}

RegisterType(Engine::Render::DirectX12TextureLoader);
