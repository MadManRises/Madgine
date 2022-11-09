#pragma once

#include "widget.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT SceneWindow : Widget<SceneWindow> {        
        SceneWindow(WidgetManager &manager, WidgetBase *parent = nullptr);

        virtual ~SceneWindow();

        virtual std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize, size_t layer) override;        

        Render::RenderTarget *getRenderTarget();

        virtual WidgetClass getClass() const override;

    protected:
        void sizeChanged(const Vector3i &pixelSize) override;

    private:
        std::unique_ptr<Render::RenderTarget> mTarget;
    };
}
}
