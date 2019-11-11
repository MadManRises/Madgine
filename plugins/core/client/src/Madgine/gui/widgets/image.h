#pragma once

#include "widget.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Widgets {
    class MADGINE_CLIENT_EXPORT Image : public Widget<Image> {
    public:
        using Widget::Widget;
        virtual ~Image() = default;

		void setImageByName(const std::string &name);
        void setImage(Resources::ImageLoader::ResourceType *image);

        std::string getImageName() const;
        Resources::ImageLoader::ResourceType *image() const;

		
		Resources::ImageLoader::ResourceType *resource() const override;

		std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

		virtual WidgetClass getClass() const override;

    private:
        Resources::ImageLoader::ResourceType *mImage = nullptr;
    };
}
}
