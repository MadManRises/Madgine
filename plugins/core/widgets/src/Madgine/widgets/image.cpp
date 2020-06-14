#include "../widgetslib.h"

#include "image.h"

#include "vertex.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "textureloader.h"

#include "widgetmanager.h"

METATABLE_BEGIN(Engine::Widgets::Image)
PROPERTY(Image, image, setImage)
METATABLE_END(Engine::Widgets::Image)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Image, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::Image)



namespace Engine {
namespace Widgets {

    void Image::setImageByName(const std::string &name)
    {
        setImage(Resources::ImageLoader::getSingleton().get(name));
    }

    void Image::setImage(Resources::ImageLoader::ResourceType *image)
    {
        mImage = image;
    }

    std::string Image::getImageName() const
    {
        return mImage ? mImage->name() : "";
    }

    Resources::ImageLoader::ResourceType *Image::image() const
    {
        return mImage;
    }

    Resources::ImageLoader::ResourceType *Image::resource() const
    {
        return mImage;
    }

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> Image::vertices(const Vector3 &screenSize)
    {
        std::vector<Vertex> result;

        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;

        Vector4 color { 1, 1, 1, 1 };

        Vector3 v = pos;
        v.z = static_cast<float>(depth());
        result.push_back({ v, color, { 0, 0 } });
        v.x += size.x;
        result.push_back({ v, color, { 1, 0 } });
        v.y += size.y;
        result.push_back({ v, color, { 1, 1 } });
        result.push_back({ v, color, { 1, 1 } });
        v.x -= size.x;
        result.push_back({ v, color, { 0, 1 } });
        v.y -= size.y;
        result.push_back({ v, color, { 0, 0 } });
        return { { result, { mImage ? manager().uiTexture().mTextureHandle : 0 } } };
    }
    WidgetClass Image::getClass() const
    {
        return WidgetClass::IMAGE_CLASS;
    }
}
}
