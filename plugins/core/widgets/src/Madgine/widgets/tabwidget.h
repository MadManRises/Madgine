#pragma once

#include "widget.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT TabWidget : Widget<TabWidget> {
        using Widget::Widget;
        virtual ~TabWidget() = default;

        virtual void addTab(Widget *w);

        virtual WidgetClass getClass() const override;
    };
}
}
