#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "texture.h"

#include "render/texturedescriptor.h"

#include "Generic/bytebuffer.h"

#include "Meta/math/vector2i.h"

namespace Engine {
namespace Render {

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
                : Base::HandleType(std::move(handle))
            {
            }

			Threading::TaskFuture<bool> create(std::string_view name, TextureType type, DataFormat format, Vector2i size = { 0, 0 }, ByteBuffer data = {}, TextureLoader *loader = nullptr);
            void loadFromImage(std::string_view name, TextureType type, DataFormat format, TextureLoader *loader = nullptr);

            void setData(Vector2i size, const ByteBuffer &data, TextureLoader *loader = nullptr);
            void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data, TextureLoader *loader = nullptr);
        };

        TextureLoader();

        virtual bool create(Texture &tex, TextureType type, DataFormat format) = 0;

        virtual void setData(Texture &tex, Vector2i size, const ByteBuffer &data) = 0;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data) = 0;
    };

}
}

RegisterType(Engine::Render::TextureLoader);