#include "../scenerendererlib.h"

#include "scenerenderdata.h"

#include "Madgine/scene/scenemanager.h"

#include "Modules/threading/awaitables/awaitablesender.h"

#include "Madgine/scene/entity/entitycomponentlist.h"

#include "Madgine/scene/entity/components/skeleton.h"

#include "Madgine/render/rendercontext.h"

namespace Engine {
namespace Render {

    SceneRenderData::SceneRenderData(Scene::SceneManager &scene)
        : mScene(scene)
    {
    }

    Threading::ImmediateTask<void> SceneRenderData::render(RenderContext *context)
    {
        auto lock = co_await mScene.mutex(Engine::AccessMode::READ);
        mScene.updateFrame();

        for (const auto &[skeleton, _] : mScene.entityComponentList<Scene::Entity::Skeleton>()) {
            if (skeleton.data() && !skeleton.mBoneMatrices.mBuffer) {
                const Render::SkeletonDescriptor *data = skeleton.data();
                skeleton.mBoneMatrices = context->allocateBuffer<Matrix4[]>(data->mBones.size());
            }
        }
    }

}
}