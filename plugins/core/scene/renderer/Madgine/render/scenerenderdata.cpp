#include "../scenerendererlib.h"

#include "scenerenderdata.h"

#include "Madgine/scene/scenemanager.h"

#include "Modules/threading/awaitables/awaitablesender.h"

namespace Engine {
namespace Render {

    SceneRenderData::SceneRenderData(Scene::SceneManager &scene)
        : mScene(scene)
    {
    }

    Threading::ImmediateTask<void> SceneRenderData::render(RenderContext *context)
    {
        auto lock = co_await mScene.mutex(Engine::AccessMode::READ);
        mScene.updateRender();
    }

}
}