#pragma once

#include "Madgine/render/renderpass.h"

#include "programloader.h"

#include "Meta/math/frustum.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT ShadowRenderPass : RenderPass {
        ShadowRenderPass(Scene::SceneManager &scene, Render::Camera *camera, int priority);

        virtual void setup(Render::RenderContext *context) override;
        virtual void shutdown() override;
        virtual void render(Render::RenderTarget *target) override;

        virtual int priority() const override;

        Matrix4 projectionMatrix() const;
        Matrix4 viewMatrix() const;

        void updateFrustum();

    private:
        ProgramLoader::HandleType mProgram;

        Scene::SceneManager &mScene;        

        Render::Camera *mCamera;

        int mPriority;

        Frustum mLightFrustum;
    };

}
}