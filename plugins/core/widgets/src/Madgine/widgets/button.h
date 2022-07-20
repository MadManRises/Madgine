#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "Madgine/fontloader/fontloader.h"
#include "Madgine/imageloader/imageloader.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        using Widget::Widget;
        virtual ~Button() = default;

        void setImageName(std::string_view name);
        void setImage(Resources::ImageLoader::Resource *image);

        std::string_view imageName() const;
        Resources::ImageLoader::Resource *image() const;

        virtual Resources::ImageLoader::Resource *resource() const override;

        Threading::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

        std::string_view fontName() const;
        void setFontName(std::string_view name);

        Render::FontLoader::Resource *font() const;
        void setFont(Render::FontLoader::Handle font);

        std::string mText;
        float mFontSize = 16;

        Vector2 mPivot = { 0.5f, 0.5f };

    protected:
        bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
        bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;

        void emitClicked();

    private:
        Render::FontLoader::Handle mFont;
        Resources::ImageLoader::Resource *mImage;

        Threading::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
