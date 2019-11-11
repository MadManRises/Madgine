#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Render {

    enum DataFormat {
		FORMAT_UNSIGNED_BYTE
    };

	enum WrapMode {
		WRAP_CLAMP_TO_EDGE
	};

    struct Texture;

    struct MADGINE_TEXTURELOADER_EXPORT TextureLoader : Resources::VirtualResourceLoaderBase<TextureLoader, Texture, Resources::ThreadLocalResource> {

        TextureLoader();

        virtual std::shared_ptr<Texture> create(DataFormat format) = 0;

        virtual void bind(const Texture &tex) = 0;
        virtual void bind(unsigned int textureHandle) = 0;

		virtual void setData(Texture &tex, Vector2i size, void *data) = 0;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, void *data) = 0;

		virtual void setWrapMode(Texture &tex, WrapMode mode) = 0;
    };

}
}