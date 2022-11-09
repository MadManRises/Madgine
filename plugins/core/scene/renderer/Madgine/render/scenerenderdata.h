#pragma once

#include "Madgine/render/renderdata.h"

namespace Engine {
namespace Render {

    struct SceneRenderData : RenderData {

        SceneRenderData(Scene::SceneManager &scene, RenderContext *context);

        virtual void render() override;

    private:
        Scene::SceneManager &mScene;
    };

}
}