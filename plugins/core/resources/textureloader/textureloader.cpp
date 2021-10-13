#include "textureloaderlib.h"

#include "textureloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/math/vector2i.h"

#include "imageloader.h"

#include "Generic/bytebuffer.h"

METATABLE_BEGIN_BASE(Engine::Render::TextureLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::TextureLoader::ResourceType)

namespace Engine {
namespace Render {

    TextureLoader::TextureLoader()
        : VirtualResourceLoaderBase(std::vector<std::string> {})
    {
    }

    void TextureLoader::HandleType::create(std::string_view name, TextureType type, DataFormat format, TextureLoader *loader)
    {
        *this = TextureLoader::loadManual(
            name, {}, [=](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceDataInfo &info) {
                return loader->create(texture, type, format);
            },
            {}, loader);
    }

    void TextureLoader::HandleType::loadFromImage(std::string_view name, TextureType type, DataFormat format, TextureLoader *loader)
    {
        *this = TextureLoader::loadManual(
            name, {}, [=](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceDataInfo &info) {
                Resources::ImageLoader::HandleType image;
                image.load(name);

                if (!image)
                    return false;
                loader->setData(texture, { image->mWidth, image->mHeight }, { image->mBuffer, static_cast<size_t>(image->mWidth * image->mHeight) });
                return true;
            },
            {}, loader);
            
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

}
}