#pragma once

#include "widget.h"

#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Widgets {
    class MADGINE_WIDGETS_EXPORT SceneWindow : public Widget<SceneWindow> {
    public:
        SceneWindow(const std::string &name, WidgetBase *parent);
        SceneWindow(const std::string &name, WidgetManager &manager);
		
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
