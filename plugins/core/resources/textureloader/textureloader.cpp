#include "textureloaderlib.h"

#include "textureloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/math/vector2i.h"

METATABLE_BEGIN_BASE(Engine::Render::TextureLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::TextureLoader::ResourceType)



    namespace Engine
{
    namespace Render {

        TextureLoader::TextureLoader()
            : VirtualResourceLoaderBase(std::vector<std::string> {})
        {
        }

        void TextureLoader::HandleType::create(std::string_view name, DataFormat format, TextureLoader *loader)
        {
            *this = TextureLoader::loadManual(
                name, {}, [=](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceDataInfo &info) { return loader->create(texture, format); }, {},
                loader);
        }

        void TextureLoader::HandleType::setData(Vector2i size, const ByteBuffer &data, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setData(*getDataPtr(*this, loader), size, data);
        }

        void TextureLoader::HandleType::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setSubData(*getDataPtr(*this, loader), offset, size, data);
        }

        void TextureLoader::HandleType::setWrapMode(WrapMode mode, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setWrapMode(*getDataPtr(*this, loader), mode);
        }

        void TextureLoader::HandleType::setMinMode(MinMode mode, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setMinMode(*getDataPtr(*this, loader), mode);
        }

    }
}