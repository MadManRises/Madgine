#include "../scenerendererlib.h"

#include "litscenerenderdata.h"

#include "scenemainwindowcomponent.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/material.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"

#include "Modules/threading/awaitables/awaitablesender.h"

namespace Engine {
namespace Render {

    LitSceneRenderData::LitSceneRenderData(SceneMainWindowComponent &scene, Camera *camera)
        : mScene(scene)
        , mCamera(camera)
    {
    }

    Threading::ImmediateTask<void> LitSceneRenderData::render(RenderContext *context)
    {
        {
            auto guard = co_await mScene.scene()->mutex(AccessMode::READ);
            assert(guard.isHeld());

            //TODO Culling

            for (auto &[key, transforms] : mInstances)
                transforms.clear();

            for (const auto &[mesh, e] : mScene.scene()->entityComponentList<Scene::Entity::Mesh>().data()) {
                if (!mesh.isVisible())
                    continue;

                const GPUMeshData *meshData = mesh.data();
                if (!meshData)
                    continue;

                Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
                if (!transform)
                    continue;

                const GPUMeshData::Material *material = nullptr;
                Scene::Entity::Material *materialComponent = e->getComponent<Scene::Entity::Material>();
                if (materialComponent) {
                    material = materialComponent->get();
                } else if (mesh.material() < meshData->mMaterials.size()) {
                    material = &meshData->mMaterials[mesh.material()];
                }

                Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();
                Engine::Render::GPUPtr<Matrix4[]> bones;
                if (skeleton)
                    bones = skeleton->mBoneMatrices;

                mInstances[std::tuple<const GPUMeshData *, const GPUMeshData::Material *> { meshData, material }].push_back({ transform->worldMatrix(), bones });
            }
        }

        for (auto it = mInstances.begin(); it != mInstances.end();) {
            if (it->second.empty()) {
                it = mInstances.erase(it);
            } else {
                ++it;
            }
        }
    }

}
}