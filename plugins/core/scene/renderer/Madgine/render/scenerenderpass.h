#pragma once

#include "Madgine/render/renderpass.h"

#include "programloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority);

        virtual void render(Render::RenderTarget *target) override;

        virtual int priority() const override;

    private:
        ProgramLoader::HandleType mProgram;

        Scene::SceneManager &mScene;

        Camera *mCamera;

        int mPriority;
    };

}
}