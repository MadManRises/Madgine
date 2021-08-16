#pragma once

#include "widget.h"

#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT SceneWindow : Widget<SceneWindow> {        
        SceneWindow(const std::string &name, WidgetManager &manager, WidgetBase *parent = nullptr);

        virtual ~SceneWindow();

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize) override;

        Render::RenderTarget *getRenderTarget();

        virtual WidgetClass getClass() const override;

    protected:
        void sizeChanged(const Vector3i &pixelSize) override;

    private:
        std::unique_ptr<Render::RenderTarget> mTarget;
    };
}
}
