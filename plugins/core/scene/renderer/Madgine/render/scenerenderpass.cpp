#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "meshdata.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Madgine/render/rendercontext.h"

#include "Madgine/render/rendertarget.h"

#define SL_SHADER scene
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    SceneRenderPass::SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority)
        : mShadowPass(scene, camera, priority + 1)
        , mScene(scene)
        , mCamera(camera)
        , mPriority(priority)
    {
    }

    SceneRenderPass::~SceneRenderPass() = default;

    void SceneRenderPass::setup(Render::RenderContext *context)
    {
        mProgram.create("scene");

        mProgram.setParameters({ nullptr, sizeof(ScenePerApplication) }, 0);
        mProgram.setParameters({ nullptr, sizeof(ScenePerFrame) }, 1);
        mProgram.setParameters({ nullptr, sizeof(ScenePerObject) }, 2);

        mShadowMap = context->createRenderTexture({ 2048, 2048 }, { .mCreateDepthBufferView = true });

        mShadowMap->addRenderPass(&mShadowPass);
    }

    void SceneRenderPass::shutdown() 
    {
        mShadowMap.reset();

        mProgram.reset();
    }

    void SceneRenderPass::render(Render::RenderTarget *target)
    {
        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mProgram.mapParameters(0).cast<ScenePerApplication>();

            perApplication->p = mCamera->getProjectionMatrix(aspectRatio);
            perApplication->lightProjectionMatrix = mShadowPass.projectionMatrix();
        }

        {
            auto perFrame = mProgram.mapParameters(1).cast<ScenePerFrame>();

            perFrame->v = mCamera->getViewMatrix();
            perFrame->lightViewMatrix = mShadowPass.viewMatrix();

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

                    target->bindTexture({ meshData->mTextureHandle, mShadowMap->depthTexture()});

                    Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

                    {
                        auto perObject = mProgram.mapParameters(2).cast<ScenePerObject>();

                        perObject->hasLight = true;

                        perObject->hasDistanceField = false;

                        perObject->hasTexture = meshData->mTextureHandle != 0;

                        Matrix4 modelMatrix = transform->worldMatrix(mScene.entityComponentList<Scene::Entity::Transform>());
                        perObject->m = modelMatrix;
                        perObject->anti_m = modelMatrix
                                                .Inverse()
                                                .Transpose();

                        perObject->hasSkeleton = skeleton != nullptr;
                    }

                    if (skeleton) {
                        mProgram.setDynamicParameters(skeleton->matrices(), 0);
                    } else {
                        mProgram.setDynamicParameters({}, 0);
                    }

                    target->renderMesh(meshData, mProgram);
                }
            }
        }
    }

    int SceneRenderPass::priority() const
    {
        return mPriority;
    }

}
}
