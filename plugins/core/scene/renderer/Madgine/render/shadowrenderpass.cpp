#include "../scenerendererlib.h"

#include "shadowrenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "meshdata.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Meta/math/transformation.h"

#include "Madgine/render/camera.h"

#include "im3d/im3d.h"

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

    void ShadowRenderPass::setup(Render::RenderContext *context)
    {
        mProgram.create("scene");

        mProgram.setParameters(0, sizeof(ScenePerApplication));
        mProgram.setParameters(1, sizeof(ScenePerFrame));
        mProgram.setParameters(2, sizeof(ScenePerObject));
    }

    void ShadowRenderPass::shutdown()
    {
        mProgram.reset();
    }

    void ShadowRenderPass::render(Render::RenderTarget *target)
    {
        updateFrustum();

        {
            auto perApplication = mProgram.mapParameters(0).cast<ScenePerApplication>();

            perApplication->p = projectionMatrix();
        }

        {
            auto perFrame = mProgram.mapParameters(1).cast<ScenePerFrame>();

            perFrame->v = viewMatrix();

            perFrame->lightColor = mScene.mAmbientLightColor;
            perFrame->lightDir = mScene.mAmbientLightDirection;
        }

        //TODO Culling

        Threading::DataLock lock { mScene.mutex(), Threading::AccessMode::READ };

        for (Engine::Scene::Entity::EntityPtr e : mScene.entities()) {
            Scene::Entity::Animation *anim = e->getComponent<Scene::Entity::Animation>();
            if (anim)
                anim->applyTransform(e);

            Scene::Entity::Mesh *mesh = e->getComponent<Scene::Entity::Mesh>();
            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                GPUMeshData *meshData = mesh->data();
                if (meshData) {

                    Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

                    {
                        auto perObject = mProgram.mapParameters(2).cast<ScenePerObject>();

                        perObject->hasLight = false;

                        perObject->hasDistanceField = false;

                        perObject->hasTexture = false;

                        Matrix4 modelMatrix = transform->worldMatrix(mScene.entityComponentList<Scene::Entity::Transform>());
                        perObject->m = modelMatrix;
                        perObject->anti_m = modelMatrix
                                                .Inverse()
                                                .Transpose();

                        perObject->hasSkeleton = skeleton != nullptr;
                    }

                    if (skeleton) {
                        mProgram.setDynamicParameters(0, skeleton->matrices());
                    } else {
                        mProgram.setDynamicParameters(0, {});
                    }

                    target->renderMesh(meshData, mProgram);
                }
            }
        }
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

    void ShadowRenderPass::updateFrustum()
    {
        Vector3 minBounds = std::numeric_limits<float>::max() * Vector3 { Vector3::UNIT_SCALE };
        Vector3 maxBounds = std::numeric_limits<float>::min() * Vector3 { Vector3::UNIT_SCALE };

        Frustum cameraFrustum = mCamera->getFrustum(1.0f);
        auto corners = cameraFrustum.getCorners();

        Quaternion q = Quaternion::FromDirection(mScene.mAmbientLightDirection);
        Quaternion qInv = q.inverse();

        for (Vector3 &v : corners) {
            v = qInv * v;
        }

        minBounds = std::accumulate(corners.begin(), corners.end(), minBounds, static_cast<Vector3 (*)(const Vector3 &, const Vector3 &)>(&min));
        maxBounds = std::accumulate(corners.begin(), corners.end(), maxBounds, static_cast<Vector3 (*)(const Vector3 &, const Vector3 &)>(&max));

        Vector3 relPos = (maxBounds + minBounds) / 2.0f;
        relPos.z = minBounds.z - 1.0f;        

        Vector2 size = maxBounds.xy() - minBounds.xy();
        
        mLightFrustum = {
            q * relPos,
            q,
            size.y / 2.0f, size.x / 2.0f,
            1.0f, maxBounds.z - minBounds.z,
            true
        };

        Im3D::Frustum(cameraFrustum, Vector4 { 0.0f, 1.0f, 0.0f, 1.0f });
        Im3D::Frustum(mLightFrustum, Vector4 { 1.0f, 0.0f, 0.0f, 1.0f });

    }

}
}
