#pragma once

#include "Madgine/scene/camera.h"

#include "Madgine/render/rendertarget.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/ray.h"

namespace Engine {
namespace Tools {

    struct SceneView : ScopeBase {

        SceneView(SceneEditor *editor, Render::RenderWindow *renderer, const ImGuiManager &manager);
        SceneView(SceneView &&) = default;
        ~SceneView();

        void render();

		Scene::Camera &camera();

    private:
        Scene::Camera mCamera;
        std::unique_ptr<Render::RenderTarget> mRenderTarget;
        const ImGuiManager &mManager;

		SceneEditor *mEditor;

		bool mMouseDown[3] = { false, false, false }, mDragging[3] = { false, false, false };
                int mDraggedAxis;
                Ray mDragStartRay;
                Scene::Entity::Transform *mDragTransform;
                Matrix4 mDragStoredMatrix;
                Vector3 mDragStoredPosition;
    };

}
}