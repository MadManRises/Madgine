#include "../../widgetslib.h"

#include "renderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::RenderData)
METATABLE_END(Engine::Widgets::RenderData)

SERIALIZETABLE_BEGIN(Engine::Widgets::RenderData)
SERIALIZETABLE_END(Engine::Widgets::RenderData)

namespace Engine {
namespace Widgets {

    bool RenderData::animationInterval(std::chrono::steady_clock::duration dur, std::chrono::steady_clock::duration on)
    {
        return std::chrono::steady_clock::now().time_since_epoch().count() % dur.count() < on.count();
    }

    void RenderData::renderQuad(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color, Vector2 topLeftUV, Vector2 bottomRightUV, bool flippedUV)
    {
        Vector2 topRightUV = { bottomRightUV.x, topLeftUV.y };
        Vector2 bottomLeftUV = { topLeftUV.x, bottomRightUV.y };

        if (flippedUV)
            std::swap(topRightUV, bottomLeftUV);

        Vector3 v = pos;
        out.push_back({ v, color, topLeftUV });
        v.x += size.x;
        out.push_back({ v, color, topRightUV });
        v.y += size.y;
        out.push_back({ v, color, bottomRightUV });
        out.push_back({ v, color, bottomRightUV });
        v.x -= size.x;
        out.push_back({ v, color, bottomLeftUV });
        v.y -= size.y;
        out.push_back({ v, color, topLeftUV });
    }

    void RenderData::renderQuadUV(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color, Rect2i rect, Vector2i textureSize, bool flippedUV)
    {
        if (flippedUV)
            std::swap(rect.mSize.x, rect.mSize.y);

        Vector2 topLeftUV = Vector2 { rect.mTopLeft + Vector2i { 1, 1 }} / Vector2 { textureSize };
        Vector2 bottomRightUV = Vector2 { rect.mTopLeft + rect.mSize - Vector2i { 1, 1 } } / Vector2 { textureSize };

        renderQuad(out, pos, size, color, topLeftUV, bottomRightUV, flippedUV);
    }

}
}