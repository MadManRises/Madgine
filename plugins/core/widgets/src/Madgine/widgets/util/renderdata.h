#pragma once

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Madgine/imageloader/imageloader.h"

#include "texturesettings.h"
#include "vertex.h"

#include "Meta/math/vector4.h"

#include "Meta/math/atlas2.h"

namespace Engine {
namespace Widgets {

    struct RenderData : Serialize::SerializableDataUnit {

        static bool animationInterval(std::chrono::steady_clock::duration dur, std::chrono::steady_clock::duration on);
        static void renderQuad(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f }, Vector2 topLeftUV = { 0.0f, 0.0f }, Vector2 bottomRightUV = { 1.0f, 1.0f }, bool flippedUV = false);
        static void renderQuadUV(std::vector<Vertex> &out, Vector3 pos, Vector2 size, Vector4 color, Rect2i rect, Vector2i textureSize, bool flippedUV = false);

    protected:
        Resources::ImageLoader::Resource *mImage = nullptr;
    };

}
}