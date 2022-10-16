#pragma once

#include "widget.h"

#include "util/textrenderdata.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Label : Widget<Label> {
        using Widget::Widget;
        virtual ~Label() = default;

        virtual WidgetClass getClass() const override;

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize, size_t layer) override;

        TextRenderData mTextRenderData;
        std::string mText;
    };
}
}

REGISTER_TYPE(Engine::Widgets::Label)
