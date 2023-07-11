#pragma once

#include "Madgine/render/renderdata.h"

namespace Engine {
namespace Render {

    struct SceneRenderData : RenderData {

        SceneRenderData(Scene::SceneManager &scene);

        virtual Threading::ImmediateTask<void> render(RenderContext *context) override;

    private:
        Scene::SceneManager &mScene;
    };

}
}