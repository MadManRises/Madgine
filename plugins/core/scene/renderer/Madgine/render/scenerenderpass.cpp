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

    SceneRenderPass::SceneRenderPass(Camera *camera)
        : mCamera(camera)
    {
        mProgram = ProgramLoader::getSingleton().getOrCreateManual("scene", [](const ProgramLoader::ResourceType *res) {
            return ProgramLoader::getSingleton().create("scene");
        });

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "tex", 0);

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "lightColor", { 1.0f, 1.0f, 1.0f });
        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "lightDir", Vector3 { 0.1f, 0.1f, 1.0f }.normalizedCopy());
    }

    void SceneRenderPass::render(Render::RenderTarget *target)
    {
        //TODO Remove this temp solution
        Engine::App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().removeQueuedEntities();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "v", mCamera->getViewMatrix());
        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "p", mCamera->getProjectionMatrix(aspectRatio));

        //TODO Culling
        for (Scene::Entity::Entity &e : App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().entities()) {

            Scene::Entity::Mesh *mesh = e.getComponent<Scene::Entity::Mesh>();
            Scene::Entity::Transform *transform = e.getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                Resources::MeshData *meshData = mesh->data();
                if (meshData) {

                    ProgramLoader::getSingleton().bind(*mProgram->loadData());

                    ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasLight", true);

                    ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasDistanceField", false);

                    ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasTexture", meshData->mTextureHandle != 0);

                    TextureLoader::getSingleton().bind(meshData->mTextureHandle);

                    ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "m", transform->matrix());
                    ProgramLoader::getSingleton().setUniform(*mProgram->loadData(),
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

}
}
