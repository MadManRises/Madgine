#pragma once

#include "widget.h"


namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT Image : Widget<Image> {
        using Widget::Widget;
        virtual ~Image() = default;

        void setImageByName(const std::string_view &name);
        void setImage(Resources::ResourceType<Resources::ImageLoader> *image);

        std::string_view getImageName() const;
        Resources::ResourceType<Resources::ImageLoader> *image() const;

        Resources::ResourceType<Resources::ImageLoader> *resource() const override;

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        virtual WidgetClass getClass() const override;

    private:
        Resources::ResourceType<Resources::ImageLoader> *mImage = nullptr;
    };
}
}

RegisterType(Engine::Widgets::Image);
