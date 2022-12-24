#pragma once

#include "vertex.h"
#include "texturesettings.h"

#include "Meta/math/color4.h"

#include "Meta/math/rect2i.h"

namespace Engine {
namespace Widgets {

    struct WidgetsVertexData {
        std::vector<Vertex> mTriangleVertices;

        void renderQuad(Vector3 pos, Vector2 size, Color4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector2 topLeftUV = { 0.0f, 0.0f }, Vector2 bottomRightUV = { 1.0f, 1.0f }, bool flippedUV = false);
        void renderQuadUV(Vector3 pos, Vector2 size, Color4 color, Rect2i rect, Vector2i textureSize, bool flippedUV = false);
    };

    struct WidgetsLinesData {
        std::vector<Vertex> mLineVertices;

        void renderLine(Vector3 posA, Vector3 posB, Color4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
    };

    struct WidgetsRenderData {
        std::map<TextureSettings, WidgetsVertexData> mVertexData;
        WidgetsLinesData mLineData;
    };

}
}