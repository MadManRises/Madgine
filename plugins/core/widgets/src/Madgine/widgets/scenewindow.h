#pragma once

#include "widget.h"

namespace Engine {
namespace Widgets {
    struct MADGINE_WIDGETS_EXPORT SceneWindow : Widget<SceneWindow> {        
        SceneWindow(WidgetManager &manager, WidgetBase *parent = nullptr);

        virtual ~SceneWindow();

        virtual void vertices(WidgetsRenderData &renderData, size_t layer) override;        

        Render::RenderTarget *getRenderTarget();

        virtual WidgetClass getClass() const override;

        bool sampled() const;

        void setSampled(bool samples);

    protected:
        void sizeChanged(const Vector3 &pixelSize) override;

    private:
        std::unique_ptr<Render::RenderTarget> mTarget;

        bool mSampled = false;
    };
}
}
