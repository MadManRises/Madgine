#include "../scenerendererlib.h"

#include "shadowrenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Madgine/render/camera.h"

//#include "Madgine/render/rendercontext.h"

#define SL_SHADER scene
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    ShadowRenderPass::ShadowRenderPass(Scene::SceneManager &scene, Render::Camera *camera, int priority)
        : mScene(scene)
        , mCamera(camera)
        , mPriority(priority)
    {
    }

    void ShadowRenderPass::setup(RenderTarget *target)
    {
        mPipeline.createDynamic({ .vs = "scene", .bufferSizes = { sizeof(ScenePerApplication), sizeof(ScenePerFrame), sizeof(ScenePerObject) }, .instanceDataSize = sizeof(SceneInstanceData) });
    }

    void ShadowRenderPass::shutdown()
    {
        mPipeline.reset();
    }

    void ShadowRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        //TODO Culling
        /* if (!mProgram.available())
            return;*/

        auto guard = mScene.lock(AccessMode::READ);

        mScene.updateRender();

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

        target->pushAnnotation("Shadow");

        updateFrustum();

        {
            auto perApplication = mPipeline->mapParameters<ScenePerApplication>(0);

            perApplication->p = projectionMatrix();
        }

        {
            auto perFrame = mPipeline->mapParameters<ScenePerFrame>(1);

            perFrame->v = viewMatrix();

            perFrame->light.light.color = mScene.mAmbientLightColor;
            perFrame->light.light.dir = mScene.mAmbientLightDirection;
        }

        for (const std::pair<const std::tuple<const GPUMeshData *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> &instance : instances) {
            const GPUMeshData *meshData = std::get<0>(instance.first);
            Scene::Entity::Skeleton *skeleton = std::get<1>(instance.first);

            {
                auto perObject = mPipeline->mapParameters<ScenePerObject>(2);

                perObject->hasLight = false;

                perObject->hasDistanceField = false;

                perObject->hasTexture = false;

                perObject->hasSkeleton = skeleton != nullptr;
            }

            std::vector<SceneInstanceData> instanceData;

            std::ranges::transform(instance.second, std::back_inserter(instanceData), [](const Matrix4 &m) {
                return SceneInstanceData {
                    m,
                    m.Inverse().Transpose()
                };
            });

            mPipeline->setInstanceData(std::move(instanceData));

            if (skeleton) {
                mPipeline->setDynamicParameters(0, skeleton->matrices());
            } else {
                mPipeline->setDynamicParameters(0, {});
            }

            mPipeline->renderMeshInstanced(instance.second.size(), meshData);
        }

        target->popAnnotation();
    }

    int ShadowRenderPass::priority() const
    {
        return mPriority;
    }

    Matrix4 ShadowRenderPass::projectionMatrix() const
    {
        return mLightFrustum.getProjectionMatrix();
    }

    Matrix4 ShadowRenderPass::viewMatrix() const
    {
        return mLightFrustum.getViewMatrix();
    }

    Matrix4 ShadowRenderPass::viewProjectionMatrix() const
    {
        return mLightFrustum.getViewProjectionMatrix();
    }

    void ShadowRenderPass::updateFrustum()
    {
        Vector3 minBounds = std::numeric_limits<float>::max() * Vector3 { Vector3::UNIT_SCALE };
        Vector3 maxBounds = std::numeric_limits<float>::lowest() * Vector3 { Vector3::UNIT_SCALE };

        Quaternion q = Quaternion::FromDirection(mScene.mAmbientLightDirection);
        Quaternion qInv = q.inverse();

        Frustum cameraFrustum = mCamera->getFrustum(1.0f);
        Frustum localFrustum = qInv * cameraFrustum;
        auto corners = localFrustum.getCorners();

        minBounds = std::accumulate(corners.begin(), corners.end(), minBounds, static_cast<Vector3 (*)(const Vector3 &, const Vector3 &)>(&min));
        maxBounds = std::accumulate(corners.begin(), corners.end(), maxBounds, static_cast<Vector3 (*)(const Vector3 &, const Vector3 &)>(&max));

        Vector3 relPos = (maxBounds + minBounds) / 2.0f;
        relPos.z = minBounds.z - 1.0f;

        Vector2 size = maxBounds.xy() - minBounds.xy();

        mLightFrustum = {
            q * relPos,
            q,
            size.y / 2.0f, size.x / 2.0f,
            1.0f, maxBounds.z - minBounds.z + 1.0f,
            true
        };
    }

}
}
