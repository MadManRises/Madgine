#pragma once

#include "Madgine/scene/camera.h"

#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Tools {

    struct SceneView : ScopeBase {

        SceneView(Render::RenderWindow *renderer, const ImGuiManager &manager);
        SceneView(SceneView &&) = default;
        ~SceneView();

        void render();

		Scene::Camera &camera();

    private:
        Scene::Camera mCamera;
        std::unique_ptr<Render::RenderTarget> mRenderTarget;
        const ImGuiManager &mManager;

		bool mMouseDown[2] = { false, false }, mDragging[2] = { false, false };
    };

}
}