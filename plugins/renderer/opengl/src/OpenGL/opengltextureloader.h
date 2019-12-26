#pragma once

#include "Modules/resources/resourceloader.h"

#include "textureloader.h"

#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTextureLoader : Resources::VirtualResourceLoaderImpl<OpenGLTextureLoader, OpenGLTexture, TextureLoader> {
        OpenGLTextureLoader();

        bool loadImpl(OpenGLTexture &tex, ResourceType *res);
        void unloadImpl(OpenGLTexture &tex, ResourceType *res);
        bool create(Texture &texture, DataFormat format) override;

        virtual void bind(const Texture &texture) override;
        virtual void bind(unsigned int textureHandle) override;

		virtual void setData(Texture &tex, Vector2i size, void *data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, void *data) override;

		virtual void setWrapMode(Texture &tex, WrapMode mode) override;
	};
}
}