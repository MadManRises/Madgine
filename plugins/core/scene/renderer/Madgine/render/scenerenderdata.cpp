#include "../scenerendererlib.h"

#include "scenerenderdata.h"

#include "Madgine/scene/scenemanager.h"

#include "Modules/threading/awaitables/awaitablesender.h"

#include "Madgine/scene/entity/entitycomponentlist.h"

#include "Madgine/scene/entity/components/skeleton.h"

#include "Madgine/render/rendercontext.h"

#include "Madgine/scene/entity/components/animation.h"

namespace Engine {
namespace Render {

    SceneRenderData::SceneRenderData(Scene::SceneManager &scene)
        : mScene(scene)
    {
    }

    RenderFuture SceneRenderData::render(RenderContext *context)
    {
        context->renderQueue()->queue([this, context]() -> Threading::ImmediateTask<void> {
            co_await mScene.mutex().locked(Engine::AccessMode::READ, [this, context]() {
                mScene.updateFrame();

                for (const auto &[skeleton, entity] : mScene.entityComponentList<Scene::Entity::Skeleton>()) {
                    if (skeleton.data()) {
                        if (!skeleton.mBoneMatrices.mBuffer) {
                            const Render::SkeletonDescriptor *data = skeleton.data();
                            skeleton.mBoneMatrices = context->allocateBuffer<Matrix4[]>(data->mBones.size());
                        }
                        auto buffer = context->mapBuffer(skeleton.mBoneMatrices);
                        if (Scene::Entity::Animation *animation = entity->getComponent<Scene::Entity::Animation>()) {
                            animation->updateRender(entity, 0ms, 0ms, buffer.mData);
                        } else {
                            skeleton.resetMatrices(buffer.mData);
                        }
                    }
                }
            });
        });

        return {};
    }

}
}