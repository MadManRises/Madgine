#pragma once

#include "renderdata.h"

namespace Engine {
namespace Widgets {

    struct ScalableImageRenderData : RenderData {

        void setImageName(std::string_view name);
        void setImage(Resources::ImageLoader::Resource *image);

        std::string_view imageName() const;
        Resources::ImageLoader::Resource *image() const;

        std::vector<Vertex> renderImage(Vector3 pos, Vector2 size, Vector2 screenSize, const Atlas2::Entry &entry, Vector4 color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f });


        uint16_t mLeftBorder = 0;
        uint16_t mTopBorder = 0;
        uint16_t mBottomBorder = 0;
        uint16_t mRightBorder = 0;

    };

}
}