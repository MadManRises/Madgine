#include "../scenerendererlib.h"

#include "scenerenderpass.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/mesh.h"
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

            mProgram.setUniform("tex", 0);

            mProgram.setUniform("lightColor", { 1.0f, 1.0f, 1.0f });
            mProgram.setUniform("lightDir", Vector3 { 0.1f, 0.1f, 1.0f }.normalizedCopy());
        }

        //TODO Remove this temp solution
        Engine::App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().removeQueuedEntities();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        mProgram.setUniform("v", mCamera->getViewMatrix());
        mProgram.setUniform("p", mCamera->getProjectionMatrix(aspectRatio));

        //TODO Culling
        for (Scene::Entity::Entity &e : App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().entities()) {

            Scene::Entity::Mesh *mesh = e.getComponent<Scene::Entity::Mesh>();
            Scene::Entity::Transform *transform = e.getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                Resources::MeshData *meshData = mesh->data();
                if (meshData) {

                    mProgram.bind();

                    mProgram.setUniform("hasLight", true);

                    mProgram.setUniform("hasDistanceField", false);

                    mProgram.setUniform("hasTexture", meshData->mTextureHandle != 0);

                    TextureLoader::getSingleton().bind(meshData->mTextureHandle);

                    mProgram.setUniform("m", transform->matrix());
                    mProgram.setUniform(
                        "anti_m",
                        transform->matrix()
                            .ToMat3()
                            .Inverse()
                            .Transpose());

                    target->renderMesh(meshData);
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
