#include "../../widgetslib.h"

#include "widgetsrenderdata.h"

namespace Engine {
namespace Widgets {

    void WidgetsVertexData::renderQuad(Vector3 pos, Vector2 size, Color4 color, Vector2 topLeftUV, Vector2 bottomRightUV, bool flippedUV)
    {
        Vector2 topRightUV = { bottomRightUV.x, topLeftUV.y };
        Vector2 bottomLeftUV = { topLeftUV.x, bottomRightUV.y };

        if (flippedUV)
            std::swap(topRightUV, bottomLeftUV);

        Vector3 v = pos;
        mTriangleVertices.push_back({ v, color, topLeftUV });
        v.x += size.x;
        mTriangleVertices.push_back({ v, color, topRightUV });
        v.y += size.y;
        mTriangleVertices.push_back({ v, color, bottomRightUV });
        mTriangleVertices.push_back({ v, color, bottomRightUV });
        v.x -= size.x;
        mTriangleVertices.push_back({ v, color, bottomLeftUV });
        v.y -= size.y;
        mTriangleVertices.push_back({ v, color, topLeftUV });
    }

    void WidgetsVertexData::renderQuadUV(Vector3 pos, Vector2 size, Color4 color, Rect2i rect, Vector2i textureSize, bool flippedUV)
    {
        if (flippedUV)
            std::swap(rect.mSize.x, rect.mSize.y);

        Vector2 topLeftUV = Vector2 { rect.mTopLeft + Vector2i { 1, 1 } } / Vector2 { textureSize };
        Vector2 bottomRightUV = Vector2 { rect.mTopLeft + rect.mSize - Vector2i { 1, 1 } } / Vector2 { textureSize };

        renderQuad(pos, size, color, topLeftUV, bottomRightUV, flippedUV);
    }

    void WidgetsLinesData::renderLine(Vector3 posA, Vector3 posB, Color4 color)
    {
        mLineVertices.push_back({ posA, color, { 0, 0 } });
        mLineVertices.push_back({ posB, color, { 0, 0 } });
    }

}
}