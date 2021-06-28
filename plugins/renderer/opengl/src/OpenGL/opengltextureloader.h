#pragma once

#include "Madgine/resources/resourceloader.h"

#include "textureloader.h"

#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLTextureLoader : Resources::VirtualResourceLoaderImpl<OpenGLTextureLoader, OpenGLTexture, TextureLoader> {
        OpenGLTextureLoader();

        bool loadImpl(OpenGLTexture &tex, ResourceDataInfo &info);
        void unloadImpl(OpenGLTexture &tex, ResourceDataInfo &info);
        bool create(Texture &texture, DataFormat format) override;

		virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) override;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) override;

		virtual void setWrapMode(Texture &tex, WrapMode mode) override;
        virtual void setMinMode(Texture &tex, MinMode mode) override;
	};
}
}

RegisterType(Engine::Render::OpenGLTextureLoader);