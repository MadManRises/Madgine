#pragma once

#include "Madgine/render/camera.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/ray.h"

#include "Madgine/render/scenerenderpass.h"

#include "Madgine_Tools/im3d/im3drenderpass.h"

#include "gridpass.h"

#include "Madgine/scene/entity/entitycomponentptr.h"
#include "Madgine/scene/entity/components/transform.h"

namespace Engine {
namespace Tools {

    struct SceneView : ScopeBase {

        SceneView(SceneEditor *editor, Render::RenderContext *renderer);
        SceneView(SceneView &&);
        ~SceneView();

        void render();

        Render::Camera &camera();

    private:
        Render::Camera mCamera;
        std::unique_ptr<Render::RenderTarget> mRenderTarget;

        SceneEditor *mEditor;

        bool mMouseDown[3] = { false, false, false };
        bool mDragging[3] = { false, false, false };
		bool mMouseClicked[3] = { false, false, false };
        int mDraggedAxis;
        Ray mDragStartRay;
        Engine::Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mDragTransform;
        Matrix4 mDragStoredMatrix;
        Vector3 mDragStoredPosition;
        Vector3 mDragRelMousePosition;

		Render::SceneRenderPass mSceneRenderer;
        GridPass mGridRenderer;
        Render::Im3DRenderPass mIm3DRenderer;
    };

}
}