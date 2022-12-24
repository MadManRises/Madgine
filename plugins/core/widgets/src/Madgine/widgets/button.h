#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "util/textrenderdata.h"
#include "util/scalableimagerenderdata.h"
#include "util/colortintrenderdata.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        
        Button(WidgetManager &manager, WidgetBase *parent = nullptr);

        virtual ~Button() = default;

        Threading::SignalStub<> &clickEvent();

        void vertices(WidgetsRenderData &renderData, size_t layer) override;

        virtual WidgetClass getClass() const override;

        TextRenderData mTextRenderData;
        ScalableImageRenderData mImageRenderData;     
        ColorTintRenderData mColorTintRenderData;

        std::string mText;

    protected:
        void injectPointerEnter(const Input::PointerEventArgs &arg) override;
        void injectPointerLeave(const Input::PointerEventArgs &arg) override;

        void injectPointerClick(const Input::PointerEventArgs &arg) override;        

        void emitClicked();

    private:
        Threading::Signal<> mClicked;

        bool mHovered = false;
    };

}
}
