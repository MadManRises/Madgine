#pragma once

#include "Modules/resources/resourceloader.h"

#include "textureloader.h"

#include "util/directx11texture.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11TextureLoader : Resources::VirtualResourceLoaderImpl<DirectX11TextureLoader, DirectX11Texture, TextureLoader> {
        DirectX11TextureLoader();

        bool loadImpl(DirectX11Texture &tex, ResourceType *res);
        void unloadImpl(DirectX11Texture &tex, ResourceType *res);
        bool create(Texture &texture, DataFormat format/*, D3D11_BIND_FLAG bind*/) override;

		virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) override;

		virtual void setWrapMode(Texture &tex, WrapMode mode) override;
        virtual void setMinMode(Texture &tex, MinMode mode) override;
	};
}
}

RegisterType(Engine::Render::DirectX11TextureLoader);
