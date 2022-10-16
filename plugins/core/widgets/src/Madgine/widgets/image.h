#pragma once

#include "widget.h"

#include "util/scalableimagerenderdata.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Image : Widget<Image> {
        using Widget::Widget;
        virtual ~Image() = default;

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize, size_t layer) override;

        virtual WidgetClass getClass() const override;

        ScalableImageRenderData mImageRenderData;
    };
}
}

REGISTER_TYPE(Engine::Widgets::Image)
