#pragma once

#include "Madgine/render/renderpass.h"

#include "Madgine/pipelineloader/pipelineloader.h"

#include "pointshadowrenderpass.h"
#include "shadowrenderpass.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority);
        SceneRenderPass(SceneRenderPass &&) = default;
        ~SceneRenderPass();

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(Render::RenderTarget *target, size_t iteration) override;
        virtual void preRender() override;

        virtual int priority() const override;

        float mAmbientFactor = 0.4f;
        float mDiffuseFactor = 0.7f;
        float mSpecularFactor = 1.0f;

    private:
        PipelineLoader::Instance mPipeline;

        std::unique_ptr<Render::RenderTarget> mShadowMap;
        std::unique_ptr<Render::RenderTarget> mPointShadowMaps[2];

        ShadowRenderPass mShadowPass;
        PointShadowRenderPass mPointShadowPasses[2];

        Scene::SceneManager &mScene;

        Camera *mCamera;

        int mPriority;
    };

}
}