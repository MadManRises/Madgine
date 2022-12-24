#pragma once

#include "widget.h"

#include "util/scalableimagerenderdata.h"
#include "util/textrenderdata.h"

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_POSTIONTYPE size_t
#define STB_TEXTEDIT_UNDOSTATECOUNT 1
#define STB_TEXTEDIT_UNDCHARCOUNT 1
#include "stb_textedit.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Textbox : Widget<Textbox> {
        Textbox(WidgetManager &manager, WidgetBase *parent = nullptr);
        virtual ~Textbox() = default;

        void setEditable(bool b);

        void vertices(WidgetsRenderData &renderData, size_t layer) override;

        virtual WidgetClass getClass() const override;

        virtual void injectPointerClick(const Input::PointerEventArgs &arg) override;        
        virtual void injectDragBegin(const Input::PointerEventArgs &arg) override;
        virtual void injectDragMove(const Input::PointerEventArgs &arg) override;
        virtual bool injectKeyPress(const Input::KeyEventArgs &arg) override;

        void layoutRow(StbTexteditRow *row, size_t i);
        float calculateWidth(size_t i, size_t n);

        TextRenderData mTextRenderData;
        std::string mText;

        ScalableImageRenderData mImageRenderData;

    private:
        STB_TexteditState mState;
    };
}
}
