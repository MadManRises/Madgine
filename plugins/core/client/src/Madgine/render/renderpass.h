#pragma once

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderPass {
        virtual ~RenderPass() = default;

        virtual void setup(RenderTarget *target) { }
        virtual void shutdown() { }
        virtual void render(RenderTarget *target, size_t iteration) = 0;
        void preRender();

        const std::vector<RenderData *> &dependencies() const;

        virtual void onTargetResize(const Vector2i &size) {};

        virtual int priority() const = 0;

    protected:
        void addDependency(RenderData *dep);
        void removeDependency(RenderData *dep);

    private:
        std::vector<RenderData *> mDependencies;
    };

}
}