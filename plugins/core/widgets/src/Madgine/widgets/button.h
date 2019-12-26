#pragma once

#include "widget.h"

#include "Modules/signalslot/signal.h"

#include "fontloader.h"

namespace Engine {
namespace Widgets {
    class MADGINE_WIDGETS_EXPORT Button : public Widget<Button> {
    public:
        using Widget::Widget;
        virtual ~Button() = default;

        SignalSlot::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
        bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;

        virtual WidgetClass getClass() const override;

		std::string getFontName() const;
        void setFontName(const std::string &name);

		Font::FontLoader::ResourceType *getFont() const;
        void setFont(Font::FontLoader::ResourceType *font);

        std::string mText;
        float mFontSize = 16;

    protected:
        void emitClicked();

    private:
        Font::FontLoader::HandleType mFont;

        SignalSlot::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
