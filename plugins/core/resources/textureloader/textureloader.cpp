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

    Threading::TaskFuture<bool> TextureLoader::HandleType::create(
        TextureType type, DataFormat format, Vector2i size, ByteBuffer data, TextureLoader * loader)
    {
        *this = TextureLoader::loadUnnamed(
            [=, data { std::move(data) }](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceDataInfo &info) mutable {
                if (!loader->create(texture, type, format))
                    return false;
                if (data.mSize > 0)
                    loader->setData(texture, size, std::move(data));
                return true;
            },
            {}, loader);
        return info()->loadingTask();
    }

    void TextureLoader::HandleType::loadFromImage(std::string_view name, TextureType type, DataFormat format, TextureLoader *loader)
    {
        *this = TextureLoader::loadManual(
            name, {}, [=](TextureLoader *loader, Texture &texture, const TextureLoader::ResourceDataInfo &info) -> Threading::Task<bool> {
                Resources::ImageLoader::HandleType image;
                if (!co_await image.load(info.resource()->name()))
                    co_return false;
                loader->setData(texture, { image->mWidth, image->mHeight }, { image->mBuffer, static_cast<size_t>(image->mWidth * image->mHeight) });
                co_return true;
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