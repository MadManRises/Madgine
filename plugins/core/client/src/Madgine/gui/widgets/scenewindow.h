#pragma once

#include "widget.h"

#include "../../render/rendertarget.h"

namespace Engine {
namespace GUI {
    class MADGINE_CLIENT_EXPORT SceneWindow : public Widget<SceneWindow> {
    public:
        using Widget::Widget;
        virtual ~SceneWindow();

        void setCamera(Scene::Camera *camera);
        Scene::Camera *camera();

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        Render::RenderTarget *getRenderTarget();

		virtual WidgetClass getClass() const override;

    protected:
        void sizeChanged(const Vector3 &pixelSize) override;

    private:
        std::unique_ptr<Render::RenderTarget> mTarget;
    };
}
}
