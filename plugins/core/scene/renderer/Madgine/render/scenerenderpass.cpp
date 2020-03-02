#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"

#include "Madgine/render/rendertarget.h"
#include "meshdata.h"

#include "textureloader.h"

namespace Engine {
namespace Render {

    SceneRenderPass::SceneRenderPass(Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {
    }

    void SceneRenderPass::render(Render::RenderTarget *target)
    {
        if (!mProgram) {
            mProgram.create("scene");

            Vector2i size = target->size();

            float aspectRatio = float(size.x) / size.y;

            mPerApplication.p = mCamera->getProjectionMatrix(aspectRatio);

            mProgram.setParameters(mPerApplication, 0);

            mPerFrame.lightColor = { 1.0f, 1.0f, 1.0f };
            mPerFrame.lightDir = Vector3 { 0.1f, 0.1f, 1.0f }.normalizedCopy();
        }

        mPerFrame.v = mCamera->getViewMatrix();

        mProgram.setParameters(mPerFrame, 1);

        //TODO Culling
        Scene::SceneManager &sceneMgr = App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>();

        Threading::DataLock lock { sceneMgr.mutex(), Threading::AccessMode::READ };

        for (Scene::Entity::Entity &e : sceneMgr.entities()) {

            Scene::Entity::Mesh *mesh = e.getComponent<Scene::Entity::Mesh>();
            Scene::Entity::Transform *transform = e.getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                MeshData *meshData = mesh->data();
                if (meshData) {
                    mPerObject.hasLight = true;

                    mPerObject.hasDistanceField = false;

                    mPerObject.hasTexture = meshData->mTextureHandle != 0;

                    TextureLoader::getSingleton().bind(meshData->mTextureHandle);

                    mPerObject.m = transform->matrix();
                    mPerObject.anti_m = transform->matrix()
                                            .Inverse()
                                            .Transpose();

                    Scene::Entity::Skeleton *skeleton = e.getComponent<Scene::Entity::Skeleton>();
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
