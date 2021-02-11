#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "texture.h"

namespace Engine {
namespace Render {

    enum DataFormat {
        FORMAT_FLOAT8,
        FORMAT_FLOAT32
    };

    enum WrapMode {
        WRAP_CLAMP_TO_EDGE		
    };

	enum MinMode {
		MIN_NEAREST
	};

    struct Texture;

    struct MADGINE_TEXTURELOADER_EXPORT TextureLoader : Resources::VirtualResourceLoaderBase<TextureLoader, Texture, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = Resources::VirtualResourceLoaderBase<TextureLoader, Texture, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct MADGINE_TEXTURELOADER_EXPORT HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

			void create(const std::string_view &name, DataFormat format, TextureLoader *loader = nullptr);

            void setData(Vector2i size, const ByteBuffer &data, TextureLoader *loader = nullptr);
            void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data, TextureLoader *loader = nullptr);

            void setWrapMode(WrapMode mode, TextureLoader *loader = nullptr);
            void setMinMode(MinMode mode, TextureLoader *loader = nullptr);
        };

        TextureLoader();

        virtual bool create(Texture &tex, DataFormat format) = 0;

        virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) = 0;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) = 0;

        virtual void setWrapMode(Texture &tex, WrapMode mode) = 0;
        virtual void setMinMode(Texture &tex, MinMode mode) = 0;
    };

}
}

RegisterType(Engine::Render::TextureLoader);