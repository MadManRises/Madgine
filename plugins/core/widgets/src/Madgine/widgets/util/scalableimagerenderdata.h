#pragma once

#include "imagerenderdata.h"

namespace Engine {
namespace Widgets {

    struct ScalableImageRenderData : ImageRenderData {

        std::vector<Vertex> renderImage(Vector3 pos, Vector2 size, Vector2 screenSize, const Atlas2::Entry &entry, Vector4 color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f });


        uint16_t mLeftBorder = 0;
        uint16_t mTopBorder = 0;
        uint16_t mBottomBorder = 0;
        uint16_t mRightBorder = 0;

    };

}
}