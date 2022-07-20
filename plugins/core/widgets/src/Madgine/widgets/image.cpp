#include "../widgetslib.h"

#include "image.h"

#include "vertex.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/imageloader/imageloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Image, Engine::Widgets::WidgetBase)
PROPERTY(Image, image, setImage)
MEMBER(mLeftBorder)
MEMBER(mTopBorder)
MEMBER(mBottomBorder)
MEMBER(mRightBorder)
METATABLE_END(Engine::Widgets::Image)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Image, Engine::Widgets::WidgetBase)
ENCAPSULATED_FIELD(Image, getImageName, setImageByName)
FIELD(mLeftBorder)
FIELD(mTopBorder)
FIELD(mBottomBorder)
FIELD(mRightBorder)
SERIALIZETABLE_END(Engine::Widgets::Image)

namespace Engine {
namespace Widgets {

    void Image::setImageByName(std::string_view name)
    {
        setImage(Resources::ImageLoader::getSingleton().get(name));
    }

    void Image::setImage(Resources::ImageLoader::Resource *image)
    {
        mImage = image;
    }

    std::string_view Image::getImageName() const
    {
        return mImage ? mImage->name() : "";
    }

    Resources::ImageLoader::Resource *Image::image() const
    {
        return mImage;
    }

    Resources::ImageLoader::Resource *Image::resource() const
    {
        return mImage;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Image::vertices(const Vector3 &screenSize)
    {
        std::vector<Vertex> result;

        Vector3 absolutePos = getEffectivePosition() * screenSize;
        Vector3 absolutePosBackup = absolutePos;
        Vector3 absoluteSize = getEffectiveSize() * screenSize;
        Vector3 absoluteSizeBackup = absoluteSize;

        Vector2 topLeftUV { 0, 0 };
        Vector2 bottomRightUV { 1, 1 };

        if (mLeftBorder > 0) {
            absolutePos.x += mLeftBorder;
            absoluteSize.x -= mLeftBorder;
            topLeftUV.x += mLeftBorder / 319.0f;
        }

        if (mTopBorder > 0) {
            absolutePos.y += mTopBorder;
            absoluteSize.y -= mTopBorder;
            topLeftUV.y += mTopBorder / 24.0f;
        }

        if (mRightBorder > 0) {
            absoluteSize.x -= mRightBorder;
            bottomRightUV.x -= mRightBorder / 319.0f;
        }

        if (mBottomBorder > 0) {
            absoluteSize.y -= mBottomBorder;
            bottomRightUV.y -= mBottomBorder / 24.0f;
        }

        Vector4 color { 1, 1, 1, 1 };

        auto renderQuad = [&](Vector3 absolutePos, Vector3 absoluteSize, Vector2 topLeftUV, Vector2 bottomRightUV) {
            Vector3 pos = absolutePos / screenSize;
            Vector3 size = absoluteSize / screenSize;

            Vector3 v = pos;
            v.z = static_cast<float>(depth());
            result.push_back({ v, color, topLeftUV });
            v.x += size.x;
            result.push_back({ v, color, { bottomRightUV.x, topLeftUV.y } });
            v.y += size.y;
            result.push_back({ v, color, bottomRightUV });
            result.push_back({ v, color, bottomRightUV });
            v.x -= size.x;
            result.push_back({ v, color, { topLeftUV.x, bottomRightUV.y } });
            v.y -= size.y;
            result.push_back({ v, color, topLeftUV });
        };

        renderQuad(absolutePos, absoluteSize, topLeftUV, bottomRightUV);

        if (mLeftBorder > 0) {
            renderQuad({ absolutePosBackup.x, absolutePos.y, absolutePos.z }, { static_cast<float>(mLeftBorder), absoluteSize.y, absoluteSize.z }, { 0.0f, topLeftUV.y }, { topLeftUV.x, bottomRightUV.y });
        }

        if (mTopBorder > 0) {
            renderQuad({ absolutePos.x, absolutePosBackup.y, absolutePos.z }, { absoluteSize.x, static_cast<float>(mTopBorder), absoluteSize.z }, { topLeftUV.x, 0.0f }, { bottomRightUV.x, topLeftUV.y });
        }

        if (mRightBorder > 0) {
            renderQuad({ absolutePos.x + absoluteSize.x, absolutePos.y, absolutePos.z }, { static_cast<float>(mRightBorder), absoluteSize.y, absoluteSize.z }, { bottomRightUV.x, topLeftUV.y }, { 1.0f, bottomRightUV.y });
        }

        if (mBottomBorder > 0) {
            renderQuad({ absolutePos.x, absolutePos.y + absoluteSize.y, absolutePos.z }, { absoluteSize.x, static_cast<float>(mBottomBorder), absoluteSize.z }, { topLeftUV.x, bottomRightUV.y }, { bottomRightUV.x, 1.0f });
        }

        if (mLeftBorder > 0 && mTopBorder > 0) {
            renderQuad(absolutePosBackup, { static_cast<float>(mLeftBorder), static_cast<float>(mTopBorder), absoluteSize.z }, { 0.0f, 0.0f }, topLeftUV);
        }

        if (mRightBorder > 0 && mBottomBorder > 0) {
            renderQuad(absolutePos + absoluteSize, { static_cast<float>(mRightBorder), static_cast<float>(mBottomBorder), absoluteSize.z }, bottomRightUV, { 1.0f, 1.0f });
        }

        if (mLeftBorder > 0 && mBottomBorder > 0) {
            renderQuad({ absolutePosBackup.x, absolutePos.y + absoluteSize.y, absolutePos.z }, { static_cast<float>(mLeftBorder), static_cast<float>(mBottomBorder), absoluteSize.z }, { 0.0f, bottomRightUV.y }, { topLeftUV.x, 1.0f });
        }

        if (mRightBorder > 0 && mTopBorder > 0) {
            renderQuad({ absolutePos.x + absoluteSize.x, absolutePosBackup.y, absolutePos.z }, { static_cast<float>(mRightBorder), static_cast<float>(mTopBorder), absoluteSize.z }, { bottomRightUV.x, 0.0f }, { 1.0f, topLeftUV.y });
        }

        return { { result, { 0 } } };
    }
    WidgetClass Image::getClass() const
    {
        return WidgetClass::IMAGE;
    }
}
}
