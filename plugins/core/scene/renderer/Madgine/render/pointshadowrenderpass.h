#pragma once

#include "Madgine/render/renderpass.h"

#include "Madgine/pipelineloader/pipelineloader.h"

#include "shadowscenerenderdata.h"


namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT PointShadowRenderPass : RenderPass {
        PointShadowRenderPass(size_t index, SceneMainWindowComponent &scene, int priority);

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(Render::RenderTarget *target, size_t iteration) override;        

        virtual int priority() const override;

    private:
        PipelineLoader::Instance mPipeline;

        ShadowSceneRenderData mData;     

        size_t mIndex;

        int mPriority;        
    };

}
}