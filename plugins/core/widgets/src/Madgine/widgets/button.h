#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "util/textrenderdata.h"
#include "util/scalableimagerenderdata.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        using Widget::Widget;
        virtual ~Button() = default;

        Threading::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

        TextRenderData mTextRenderData;
        ScalableImageRenderData mImageRenderData;        

        std::string mText;

    protected:
        bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
        bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

        bool injectPointerClick(const Input::PointerEventArgs &arg) override;        

        void emitClicked();

    private:
        Threading::Signal<> mClicked;

        bool mHovered = false;
    };

}
}
