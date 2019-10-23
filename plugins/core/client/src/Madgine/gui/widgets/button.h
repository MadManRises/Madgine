#pragma once

#include "widget.h"

#include "Modules/signalslot/signal.h"

namespace Engine {
namespace GUI {
    class MADGINE_CLIENT_EXPORT Button : public Widget<Button> {
    public:
        using Widget::Widget;
        virtual ~Button() = default;

        SignalSlot::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
        bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;

        virtual WidgetClass getClass() const override;

		std::string getFontName() const;
        void setFontName(const std::string &name);

        std::string mText;
        float mFontSize = 16;
        Resources::ThreadLocalResource<Font::Font> *mFont = nullptr;

    protected:
        void emitClicked();

    private:
        SignalSlot::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
