#pragma once

#include "Madgine/resources/resourceloader.h"

#include "textureloader.h"

#include "util/directx11texture.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11TextureLoader : Resources::VirtualResourceLoaderImpl<DirectX11TextureLoader, DirectX11Texture, TextureLoader> {
        DirectX11TextureLoader();

        bool loadImpl(DirectX11Texture &tex, ResourceDataInfo &info);
        void unloadImpl(DirectX11Texture &tex);
        bool create(Texture &texture, TextureType type, DataFormat format/*, D3D11_BIND_FLAG bind*/) override;

		virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
	};
}
}

RegisterType(Engine::Render::DirectX11TextureLoader);
