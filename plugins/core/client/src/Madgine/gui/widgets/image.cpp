#include "../../clientlib.h"

#include "image.h"

#include "../vertex.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

METATABLE_BEGIN(Engine::GUI::Image)
PROPERTY(Image, image, setImage)
METATABLE_END(Engine::GUI::Image)

RegisterType(Engine::GUI::Image);

namespace Engine {
namespace GUI {

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

    std::pair<std::vector<Vertex>, uint32_t> Image::vertices(const Vector3 &screenSize)
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
        return { result, mImage ? std::numeric_limits<uint32_t>::max() : 0 };
    }
}
}
