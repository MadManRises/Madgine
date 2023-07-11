#pragma once

#include "Madgine/render/renderdata.h"

#include "Madgine/meshloader/gpumeshdata.h"

namespace Engine {
namespace Render {

    struct LitSceneRenderData : RenderData {

        LitSceneRenderData(SceneMainWindowComponent &scene, Camera *camera);

        virtual Threading::ImmediateTask<void> render(RenderContext *context) override;

        SceneMainWindowComponent &mScene;

        Camera *mCamera;

        std::map<std::tuple<const GPUMeshData *, const GPUMeshData::Material *, Scene::Entity::Skeleton *>, std::vector<Matrix4>> mInstances;
    };

}
}