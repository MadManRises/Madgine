#pragma once

#include "Madgine/render/renderpass.h"

#include "programloader.h"

#include "shadowrenderpass.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority);
        SceneRenderPass(SceneRenderPass &&) = default;
        ~SceneRenderPass();

        virtual void setup(Render::RenderContext *context) override;
        virtual void shutdown() override;
        virtual void render(Render::RenderTarget *target) override;

        virtual int priority() const override;

    private:
        ProgramLoader::HandleType mProgram;

        std::unique_ptr<Render::RenderTarget> mShadowMap;

        ShadowRenderPass mShadowPass;

        Scene::SceneManager &mScene;

        Camera *mCamera;

        int mPriority;
    };

}
}