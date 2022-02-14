#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "fontloader.h"
#include "imageloader.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        using Widget::Widget;
        virtual ~Button() = default;

        void setImageName(std::string_view name);
        void setImage(Resources::ImageLoader::ResourceType *image);

        std::string_view imageName() const;
        Resources::ImageLoader::ResourceType *image() const;

        virtual Resources::ImageLoader::ResourceType *resource() const override;

        Threading::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

        std::string_view fontName() const;
        void setFontName(std::string_view name);

        Render::FontLoader::ResourceType *font() const;
        void setFont(Render::FontLoader::HandleType font);

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
        Render::FontLoader::HandleType mFont;
        Resources::ImageLoader::ResourceType *mImage;

        Threading::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
