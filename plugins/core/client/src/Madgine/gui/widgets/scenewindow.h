#pragma once

#include "widget.h"

#include "../../render/rendertarget.h"

namespace Engine {
namespace Widgets {
    class MADGINE_CLIENT_EXPORT SceneWindow : public Widget<SceneWindow> {
    public:
       using Widget::Widget;
        virtual ~SceneWindow();

        std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        Render::RenderTarget *getRenderTarget();

		virtual WidgetClass getClass() const override;

    protected:
        void sizeChanged(const Vector3i &pixelSize) override;

    private:
        std::unique_ptr<Render::RenderTarget> mTarget;
    };
}
}
