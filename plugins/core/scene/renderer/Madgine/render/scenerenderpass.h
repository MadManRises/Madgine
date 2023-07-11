#pragma once

#include "Madgine/render/renderpass.h"

#include "Madgine/pipelineloader/pipelineloader.h"

#include "pointshadowrenderpass.h"
#include "shadowrenderpass.h"

#include "litscenerenderdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(SceneMainWindowComponent &scene, Camera *camera, int priority);
        SceneRenderPass(SceneRenderPass &&);
        ~SceneRenderPass();

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(Render::RenderTarget *target, size_t iteration) override;

        virtual int priority() const override;

        float mAmbientFactor = 0.4f;
        float mDiffuseFactor = 0.7f;
        float mSpecularFactor = 1.0f;

    private:
        PipelineLoader::Instance mPipeline;

        LitSceneRenderData mData;


        int mPriority;
    };

}
}