#include "../../widgetslib.h"

#include "imagerenderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::ImageRenderData)
PROPERTY(Image, image, setImage)
METATABLE_END(Engine::Widgets::ImageRenderData)

SERIALIZETABLE_BEGIN(Engine::Widgets::ImageRenderData)
ENCAPSULATED_FIELD(Image, imageName, setImageName)
SERIALIZETABLE_END(Engine::Widgets::ImageRenderData)

namespace Engine {
namespace Widgets {

    void ImageRenderData::setImageName(std::string_view name)
    {
        setImage(Resources::ImageLoader::getSingleton().get(name));
    }

    void ImageRenderData::setImage(Resources::ImageLoader::Resource *image)
    {
        mImage = image;
    }

    std::string_view ImageRenderData::imageName() const
    {
        return mImage ? mImage->name() : "";
    }

    Resources::ImageLoader::Resource *ImageRenderData::image() const
    {
        return mImage;
    }

    std::vector<Vertex> ImageRenderData::renderImage(Vector3 pos, Vector3 size, const Atlas2::Entry &entry, Vector4 color)
    {
        std::vector<Vertex> result;

        Vector2 topLeftUV = Vector2 { entry.mArea.mTopLeft } / (2048.f /* * mData->mUIAtlasSize*/);
        Vector2 bottomRightUV = Vector2 { entry.mArea.mTopLeft + entry.mArea.mSize } / (2048.f /* * mData->mUIAtlasSize*/);

        renderQuad(result, pos, size.xy(), color, topLeftUV, bottomRightUV);
        return result;
    }

    void ImageRenderData::renderQuad(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color, Vector2 topLeftUV, Vector2 bottomRightUV)
    {
        Vector3 v = pos;
        out.push_back({ v, color, topLeftUV });
        v.x += size.x;
        out.push_back({ v, color, { bottomRightUV.x, topLeftUV.y } });
        v.y += size.y;
        out.push_back({ v, color, bottomRightUV });
        out.push_back({ v, color, bottomRightUV });
        v.x -= size.x;
        out.push_back({ v, color, { topLeftUV.x, bottomRightUV.y } });
        v.y -= size.y;
        out.push_back({ v, color, topLeftUV });
    }

}
}