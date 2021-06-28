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

#include "Madgine/render/camera.h"


#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER scene
#include INCLUDE_SL_SHADER


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

            mProgram.setParameters({ nullptr, sizeof(ScenePerApplication) }, 0);
            mProgram.setParameters({ nullptr, sizeof(ScenePerFrame) }, 1);
            mProgram.setParameters({ nullptr, sizeof(ScenePerObject) }, 2);
        }

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        auto perApplication = mProgram.mapParameters(0).cast<ScenePerApplication>();

        perApplication->p = mCamera->getProjectionMatrix(aspectRatio);

        auto perFrame = mProgram.mapParameters(1).cast<ScenePerFrame>();
        
        perFrame->v = mCamera->getViewMatrix();     

        perFrame->lightColor = { 1.0f, 1.0f, 1.0f };
        perFrame->lightDir = Vector3 { 0.2f, -0.2f, 1.0f }.normalizedCopy();

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

                    target->bindTexture(meshData->mTextureHandle);

                    Scene::Entity::Skeleton *skeleton = e->getComponent<Scene::Entity::Skeleton>();

                    auto perObject = mProgram.mapParameters(2).cast<ScenePerObject>();

                    perObject->hasLight = true;

                    perObject->hasDistanceField = false;

                    perObject->hasTexture = meshData->mTextureHandle != 0;

                    perObject->m = transform->worldMatrix(mScene.entityComponentList<Scene::Entity::Transform>());
                    perObject->anti_m = perObject->m.mValue
                                            .Inverse()
                                            .Transpose();
                    
                    perObject->hasSkeleton = skeleton != nullptr;


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
