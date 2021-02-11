#pragma once

#include "Generic/virtualbase.h"
#include "widget.h"

#include "fontloader.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Label : Widget<Label> {
        using Widget::Widget;
        virtual ~Label() = default;

        virtual WidgetClass getClass() const override;

        std::string_view getFontName() const;
        void setFontName(const std::string_view &name);

        Render::FontLoader::ResourceType *getFont() const;
        void setFont(Render::FontLoader::ResourceType *font);

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        std::string mText;
        int mFontSize = 16;

    private:
        Render::FontLoader::HandleType mFont;
    };
}
}

RegisterType(Engine::Widgets::Label);
