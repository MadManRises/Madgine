#include "textureloaderlib.h"

#include "textureloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

METATABLE_BEGIN_BASE(Engine::Render::TextureLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::TextureLoader::ResourceType)

RegisterType(Engine::Render::TextureLoader)

    namespace Engine
{
    namespace Render {

        TextureLoader::TextureLoader()
            : VirtualResourceLoaderBase(std::vector<std::string> {})
        {
        }

        void TextureLoader::HandleType::create(const std::string &name, DataFormat format, TextureLoader *loader)
        {
            *this = TextureLoader::loadManual(
                name, {}, [=](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceType *res) { return loader->create(texture, format); }, {},
                loader);
        }

        void TextureLoader::HandleType::bind(TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->bind(getData(*this, loader));
        }

        void TextureLoader::HandleType::setData(Vector2i size, void *data, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setData(getData(*this, loader), size, data);
        }

        void TextureLoader::HandleType::setSubData(Vector2i offset, Vector2i size, void *data, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setSubData(getData(*this, loader), offset, size, data);
        }

        void TextureLoader::HandleType::setWrapMode(WrapMode mode, TextureLoader *loader)
        {
            if (!loader)
                loader = &TextureLoader::getSingleton();
            loader->setWrapMode(getData(*this, loader), mode);
        }

    }
}