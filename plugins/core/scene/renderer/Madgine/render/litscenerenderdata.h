#pragma once

#include "Madgine/render/renderdata.h"

#include "Madgine/meshloader/gpumeshdata.h"

#include "Madgine/render/ptr.h"

namespace Engine {
namespace Render {

    struct LitSceneRenderData : RenderData {

        LitSceneRenderData(SceneMainWindowComponent &scene, Camera *camera);

        virtual RenderFuture render(RenderContext *context) override;

        SceneMainWindowComponent &mScene;

        Camera *mCamera;

        struct ObjectData {
            Matrix4 mTransform;
            GPUPtr<Matrix4[]> mBones;
        };
        std::map<std::tuple<const GPUMeshData *, const GPUMeshData::Material *>, std::vector<ObjectData>> mInstances;
    };

}
}