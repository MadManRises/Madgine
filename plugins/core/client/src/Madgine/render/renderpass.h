#pragma once

namespace Engine {
namespace Render {

    struct RenderPass {
        virtual ~RenderPass() = default;

        virtual void setup(RenderTarget *target) { }
        virtual void shutdown() { }
        virtual void render(RenderTarget *target, size_t iteration) = 0;
        virtual void preRender() { }        

        virtual void onTargetResize(const Vector2i &size) {};

        virtual int priority() const = 0;
    };

}
}