#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "fontloader.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        using Widget::Widget;
        virtual ~Button() = default;

        void setImageByName(std::string_view name);
        void setImage(Resources::ResourceType<Resources::ImageLoader> *image);

        std::string_view getImageName() const;
        Resources::ResourceType<Resources::ImageLoader> *image() const;

        Resources::ResourceType<Resources::ImageLoader> *resource() const override;

        Threading::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

        std::string_view getFontName() const;
        void setFontName(std::string_view name);

        Render::FontLoader::ResourceType *getFont() const;
        void setFont(Render::FontLoader::ResourceType *font);

        std::string mText;
        float mFontSize = 16;

    protected:
        bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
        bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;

        void emitClicked();

    private:
        Render::FontLoader::HandleType mFont;
        //TODO: Use HandleType
        Resources::ResourceType<Resources::ImageLoader> *mImage = nullptr;

        Threading::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
