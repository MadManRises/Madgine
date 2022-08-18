#include "../scenerendererlib.h"

#include "pointshadowrenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/pointlight.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Meta/math/transformation.h"

#include "Meta/math/frustum.h"

//#include "Madgine/render/rendercontext.h"

#define SL_SHADER pointshadow
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    PointShadowRenderPass::PointShadowRenderPass(size_t index, Scene::SceneManager &scene, int priority)
        : mScene(scene)
        , mIndex(index)
        , mPriority(priority)
    {
    }

    void PointShadowRenderPass::setup(RenderTarget *target)
    {
        mPipeline.createDynamic({ .vs = "pointshadow", .ps = "pointshadow", .gs = "pointshadow", .bufferSizes = { sizeof(PointShadowPerApplication), sizeof(PointShadowPerFrame), sizeof(PointShadowPerObject) }, .instanceDataSize = sizeof(PointShadowInstanceData) });
    }

    void PointShadowRenderPass::shutdown()
    {
        mPipeline.reset();
    }

    void PointShadowRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        Scene::Entity::EntityComponentList<Scene::Entity::PointLight> &lights = mScene.entityComponentList<Scene::Entity::PointLight>();
        if (mIndex >= lights.size())
            return;

        //TODO Culling

        auto guard = mScene.lock(AccessMode::READ);

        mScene.updateRender();

        Scene::Entity::Entity *entity = lights.getEntity(mIndex);
        Scene::Entity::Transform *transform = entity->getComponent<Scene::Entity::Transform>();
        if (!transform)
            return;

        std::map<std::tuple<const GPUMeshData *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> instances;

        for (const auto &[mesh, e] : mScene.entityComponentList<Scene::Entity::Mesh>().data()) {
            if (!mesh.isVisible())
                continue;

            const GPUMeshData *meshData = mesh.data();
            if (!meshData)
                continue;

            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (!transform)
                continue;

            Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

            instances[std::tuple<const GPUMeshData *, Scene::Entity::Skeleton *> { meshData, skeleton }].push_back(transform->worldMatrix());
        }

        target->pushAnnotation("PointShadow");

        {
            auto perApplication = mPipeline->mapParameters<PointShadowPerApplication>(0);

            Frustum f {
                Vector3::ZERO,
                Quaternion {},
                0.01f, 0.01f,
                0.01f, 100.0f,
                false
            };
            perApplication->p = ProjectionMatrix(f);
        }

        {
            auto perFrame = mPipeline->mapParameters<PointShadowPerFrame>(1);

            perFrame->position = transform->getPosition();
        }

        for (std::pair<const std::tuple<const GPUMeshData *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> &instance : instances) {
            const GPUMeshData *meshData = std::get<0>(instance.first);
            Scene::Entity::Skeleton *skeleton = std::get<1>(instance.first);

            {
                auto perObject = mPipeline->mapParameters<PointShadowPerObject>(2);

                perObject->hasSkeleton = skeleton != nullptr;
            }

            size_t instanceCount = instance.second.size();
            mPipeline->setInstanceData(std::move(instance.second));

            if (skeleton) {
                mPipeline->setDynamicParameters(0, skeleton->matrices());
            } else {
                mPipeline->setDynamicParameters(0, {});
            }

            mPipeline->renderMeshInstanced(instanceCount, meshData);
        }

        target->popAnnotation();
    }

    int PointShadowRenderPass::priority() const
    {
        return mPriority;
    }

}
}
