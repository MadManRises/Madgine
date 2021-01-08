#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "meshdata.h"

namespace Engine {
namespace Render {

    SceneRenderPass::SceneRenderPass(Scene::SceneManager &scene, Camera *camera, int priority)
        : mScene(scene)
        , mCamera(camera)
        , mPriority(priority)
    {
    }

    void SceneRenderPass::render(Render::RenderTarget *target)
    {
        if (!mProgram) {
            mProgram.create("scene");

            mPerFrame.lightColor = { 1.0f, 1.0f, 1.0f };
            mPerFrame.lightDir = Vector3 { 0.2f, -0.2f, 1.0f }.normalizedCopy();
        }

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        mPerApplication.p = mCamera->getProjectionMatrix(aspectRatio);

        mProgram.setParameters(mPerApplication, 0);

        mPerFrame.v = mCamera->getViewMatrix();

        mProgram.setParameters(mPerFrame, 1);

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
                    mPerObject.hasLight = true;

                    mPerObject.hasDistanceField = false;

                    mPerObject.hasTexture = meshData->mTextureHandle != 0;

                    target->bindTexture(meshData->mTextureHandle);

                    mPerObject.m = transform->worldMatrix(mScene.entityComponentList<Scene::Entity::Transform>());
                    mPerObject.anti_m = mPerObject.m.mValue
                                            .Inverse()
                                            .Transpose();

                    Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();
                    mPerObject.hasSkeleton = skeleton != nullptr;

                    mProgram.setParameters(mPerObject, 2);

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
