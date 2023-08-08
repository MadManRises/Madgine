#include "../scenerendererlib.h"

#include "shadowrenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "Madgine/render/rendercontext.h"

#include "Madgine/render/camera.h"

#include "scenemainwindowcomponent.h"

//#include "Madgine/render/rendercontext.h"

#include "Madgine/render/shadinglanguage/sl_support_begin.h"
#include "shaders/scene.sl"
#include "Madgine/render/shadinglanguage/sl_support_end.h"

namespace Engine {
namespace Render {

    ShadowRenderPass::ShadowRenderPass(SceneMainWindowComponent &scene, Render::Camera *camera, int priority)
        : mData(scene, camera)        
        , mPriority(priority)
    {
    }

    void ShadowRenderPass::setup(RenderTarget *target)
    {
        mPipeline.create({ .vs = "scene", .bufferSizes = { sizeof(ScenePerApplication), sizeof(ScenePerFrame), sizeof(ScenePerObject) }, .instanceDataSize = sizeof(SceneInstanceData) });

        addDependency(&mData);
    }

    void ShadowRenderPass::shutdown()
    {
        removeDependency(&mData);

        mPipeline.reset();
    }

    void ShadowRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {

        if (!mPipeline.available())
            return;

        
        target->pushAnnotation("Shadow");

        Vector2i size = target->size();
        updateFrustum(1.0f);

        {
            auto perApplication = mPipeline->mapParameters<ScenePerApplication>(0);

            perApplication->p = target->getClipSpaceMatrix() * projectionMatrix();
        }

        Matrix4 v = viewMatrix();

        {
            auto perFrame = mPipeline->mapParameters<ScenePerFrame>(1);

            perFrame->light.light.color = mData.mScene.mAmbientLightColor;
            perFrame->light.light.dir = (v * Vector4{mData.mScene.mAmbientLightDirection, 0.0f}).xyz();
        }

        for (const std::pair<const std::tuple<const GPUMeshData *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> &instance : mData.mInstances) {
            const GPUMeshData *meshData = std::get<0>(instance.first);
            Scene::Entity::Skeleton *skeleton = std::get<1>(instance.first);

            {
                auto perObject = mPipeline->mapParameters<ScenePerObject>(2);

                perObject->hasLight = false;

                perObject->hasDistanceField = false;

                perObject->hasTexture = false;

                perObject->hasSkeleton = skeleton != nullptr;
            }

            if (skeleton) {
                mPipeline->setDynamicParameters(0, skeleton->matrices());
            } else {
                mPipeline->setDynamicParameters(0, {});
            }

            std::vector<SceneInstanceData> instanceData;

            std::ranges::transform(instance.second, std::back_inserter(instanceData), [&](const Matrix4 &m) {
                Matrix4 mv = v * m;
                return SceneInstanceData {
                    mv.Transpose(),
                    mv.Inverse().Transpose().Transpose()
                };
            });

            mPipeline->renderMeshInstanced(target, std::move(instanceData), meshData);
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

    void ShadowRenderPass::updateFrustum(float aspectRatio)
    {
        Vector3 minBounds = std::numeric_limits<float>::max() * Vector3 { Vector3::UNIT_SCALE };
        Vector3 maxBounds = std::numeric_limits<float>::lowest() * Vector3 { Vector3::UNIT_SCALE };

        Quaternion q = Quaternion::FromDirection(mData.mScene.mAmbientLightDirection);
        Quaternion qInv = q.inverse();

        Frustum cameraFrustum = mData.mCamera->getFrustum(aspectRatio);
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

    void ShadowRenderPass::debugFrustums(Lambda<void(const Frustum &, std::string_view)> handler) const
    {
        handler(mLightFrustum, "ShadowRenderPass");
    }

}
}
