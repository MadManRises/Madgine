#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    enum DataFormat {
        FORMAT_UNSIGNED_BYTE
    };

    enum WrapMode {
        WRAP_CLAMP_TO_EDGE
    };

    struct Texture;

    struct MADGINE_TEXTURELOADER_EXPORT TextureLoader : Resources::VirtualResourceLoaderBase<TextureLoader, Texture, std::vector<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = Resources::VirtualResourceLoaderBase<TextureLoader, Texture, std::vector<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct MADGINE_TEXTURELOADER_EXPORT HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

			void create(const std::string &name, DataFormat format, TextureLoader *loader = nullptr);

            void bind(TextureLoader *loader = nullptr);

            void setData(Vector2i size, void *data, TextureLoader *loader = nullptr);
            void setSubData(Vector2i offset, Vector2i size, void *data, TextureLoader *loader = nullptr);

            void setWrapMode(WrapMode mode, TextureLoader *loader = nullptr);
        };

        TextureLoader();

        virtual bool create(Texture &tex, DataFormat format) = 0;

        virtual void bind(const Texture &tex) = 0;
        virtual void bind(unsigned int textureHandle) = 0;

        virtual void setData(Texture &tex, Vector2i size, void *data) = 0;
        virtual void setSubData(Texture &tex, Vector2i offset, Vector2i size, void *data) = 0;

        virtual void setWrapMode(Texture &tex, WrapMode mode) = 0;
    };

}
}