#pragma once

#include "Modules/resources/resourceloader.h"

#include "textureloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTextureLoader : Resources::VirtualResourceLoaderImpl<OpenGLTextureLoader, TextureLoader> {
        OpenGLTextureLoader();

        std::shared_ptr<Texture> loadImpl(ResourceType *res) override;
        std::shared_ptr<Texture> create(DataFormat format) override;

        virtual void bind(const Texture &texture) override;
        virtual void bind(unsigned int textureHandle) override;

		virtual void setData(Texture &tex, Vector2i size, void *data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, void *data) override;

		virtual void setWrapMode(Texture &tex, WrapMode mode) override;
	};
}
}