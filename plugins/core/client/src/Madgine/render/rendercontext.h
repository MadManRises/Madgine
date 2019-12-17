#pragma once

#include "Modules/math/vector2i.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderContext {
        RenderContext();
        virtual ~RenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::Window *w) = 0;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }) = 0;

        void addRenderTarget(RenderTarget *target);
        void removeRenderTarget(RenderTarget *target);

        void render();

    protected:
        std::vector<RenderTarget *> mRenderTargets;
    };

}
}