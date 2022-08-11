#include "../../widgetslib.h"

#include "scalableimagerenderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::ScalableImageRenderData, Engine::Widgets::ImageRenderData)
MEMBER(mLeftBorder)
MEMBER(mTopBorder)
MEMBER(mBottomBorder)
MEMBER(mRightBorder)
METATABLE_END(Engine::Widgets::ScalableImageRenderData)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::ScalableImageRenderData, Engine::Widgets::ImageRenderData)
FIELD(mLeftBorder)
FIELD(mTopBorder)
FIELD(mBottomBorder)
FIELD(mRightBorder)
SERIALIZETABLE_END(Engine::Widgets::ScalableImageRenderData)

namespace Engine {
namespace Widgets {

    std::vector<Vertex> ScalableImageRenderData::renderImage(Vector3 pos, Vector2 size, Vector2 screenSize, const Atlas2::Entry &entry, Vector4 color)
    {
        std::vector<Vertex> result;
        Vector3 posOuter = pos;
        
        Vector2 topLeftUV = Vector2 { entry.mArea.mTopLeft } / (2048.f /* * mData->mUIAtlasSize*/);
        Vector2 uvSize = Vector2 { entry.mArea.mSize } / (2048.f /* * mData->mUIAtlasSize*/);
        Vector2 bottomRightUV = topLeftUV + uvSize;

        Vector2 topLeftUVOuter = topLeftUV;
        Vector2 bottomRightUVOuter = bottomRightUV;

        const Vector2i &imageSize = entry.mArea.mSize;

        float scaledLeft = mLeftBorder / screenSize.x;
        float scaledTop = mTopBorder / screenSize.y;
        float scaledRight = mRightBorder / screenSize.x;
        float scaledBottom = mBottomBorder / screenSize.y;

        if (mLeftBorder > 0) {
            pos.x += scaledLeft;
            size.x -= scaledLeft;
            topLeftUV.x += mLeftBorder * uvSize.x / imageSize.x;
        }

        if (mTopBorder > 0) {
            pos.y += scaledTop;
            size.y -= scaledTop;
            topLeftUV.y += mTopBorder * uvSize.y / imageSize.y;
        }

        if (mRightBorder > 0) {
            size.x -= scaledRight;
            bottomRightUV.x -= mRightBorder * uvSize.x / imageSize.x;
        }

        if (mBottomBorder > 0) {
            size.y -= scaledBottom;
            bottomRightUV.y -= mBottomBorder * uvSize.y / imageSize.y;
        }

        renderQuad(result, pos, size, color, topLeftUV, bottomRightUV);

        if (mLeftBorder > 0)
            renderQuad(result, { posOuter.x, pos.y, pos.z }, { scaledLeft, size.y }, color, { topLeftUVOuter.x, topLeftUV.y }, { topLeftUV.x, bottomRightUV.y });

        if (mTopBorder > 0)
            renderQuad(result, { pos.x, posOuter.y, pos.z }, { size.x, scaledTop }, color, { topLeftUV.x, topLeftUVOuter.y }, { bottomRightUV.x, topLeftUV.y });

        if (mRightBorder > 0)
            renderQuad(result, { pos.x + size.x, pos.y, pos.z }, { scaledRight, size.y }, color, { bottomRightUV.x, topLeftUV.y }, { bottomRightUVOuter.x, bottomRightUV.y });

        if (mBottomBorder > 0)
            renderQuad(result, { pos.x, pos.y + size.y, pos.z }, { size.x, scaledBottom }, color, { topLeftUV.x, bottomRightUV.y }, { bottomRightUV.x, bottomRightUVOuter.y });

        if (mLeftBorder > 0 && mTopBorder > 0)
            renderQuad(result, posOuter, { scaledLeft, scaledTop }, color, topLeftUVOuter, topLeftUV);

        if (mRightBorder > 0 && mBottomBorder > 0)
            renderQuad(result, { pos.x + size.x, pos.y + size.y, pos.z }, { scaledRight, scaledBottom }, color, bottomRightUV, bottomRightUVOuter);

        if (mLeftBorder > 0 && mBottomBorder > 0)
            renderQuad(result, { posOuter.x, pos.y + size.y, pos.z }, { scaledLeft, scaledBottom }, color, { topLeftUVOuter.x, bottomRightUV.y }, { topLeftUV.x, bottomRightUVOuter.y });

        if (mRightBorder > 0 && mTopBorder > 0)
            renderQuad(result, { pos.x + size.x, posOuter.y, pos.z }, { scaledRight, scaledTop }, color, { bottomRightUV.x, topLeftUVOuter.y }, { bottomRightUVOuter.x, topLeftUV.y });

        return result;
    }

}
}
