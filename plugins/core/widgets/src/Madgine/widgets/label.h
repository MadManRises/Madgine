#pragma once

#include "Modules/generic/virtualbase.h"
#include "widget.h"

namespace Engine {
namespace Widgets {
    class MADGINE_WIDGETS_EXPORT Label : public Widget<Label> {
    public:
        using Widget::Widget;
        virtual ~Label() = default;

        virtual WidgetClass getClass() const override;

        std::string getFontName() const;
        void setFontName(const std::string &name);

        std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        std::string mText;
        int mFontSize = 16;
        Resources::ThreadLocalResource<Font::Font> *mFont = nullptr;
    };
}
}
