#pragma once

#include "widget.h"

#include "Modules/threading/signal.h"

#include "fontloader.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Button : Widget<Button> {
        using Widget::Widget;
        virtual ~Button() = default;

         void setImageByName(const std::string &name);
        void setImage(Resources::ImageLoader::ResourceType *image);

        std::string getImageName() const;
        Resources::ImageLoader::ResourceType *image() const;

                Resources::ImageLoader::ResourceType *resource() const override;

        Threading::SignalStub<> &clickEvent();

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

        std::string getFontName() const;
        void setFontName(const std::string &name);

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
        Resources::ImageLoader::ResourceType *mImage = nullptr;

        Threading::Signal<> mClicked;

        bool mHovered = false;
        bool mClicking = false;
    };

}
}
