#pragma once

#include "Madgine/render/renderpass.h"

#include "Madgine/pipelineloader/pipelineloader.h"

#include "Meta/math/frustum.h"

#include "shadowscenerenderdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT ShadowRenderPass : RenderPass {
        ShadowRenderPass(SceneMainWindowComponent &scene, Render::Camera *camera, int priority);

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(RenderTarget *target, size_t iteration) override;        

        virtual int priority() const override;

        Matrix4 projectionMatrix() const;
        Matrix4 viewMatrix() const;
        Matrix4 viewProjectionMatrix() const;

        void updateFrustum();

    private:
        PipelineLoader::Instance mPipeline;

        ShadowSceneRenderData mData;

        int mPriority;

        Frustum mLightFrustum;
    };

}
}