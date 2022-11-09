#include "../scenerendererlib.h"

#include "scenerenderdata.h"

#include "Madgine/scene/scenemanager.h"

namespace Engine {
namespace Render {

    SceneRenderData::SceneRenderData(Scene::SceneManager &scene, RenderContext *context)
        : RenderData(context)
        , mScene(scene)
    {
    }

    void SceneRenderData::render()
    {
        auto lock = mScene.lock(Engine::AccessMode::READ);
        mScene.updateRender();
    }

}
}