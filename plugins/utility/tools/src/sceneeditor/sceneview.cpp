#include "../toolslib.h"
#include "OpenGL/opengllib.h"

#include "sceneview.h"

#include "../imgui/imgui.h"
#include "../renderer/imguiroot.h"

#include "../renderer/imguiaddons.h"

#include "Madgine/render/renderwindow.h"

#include "../renderer/imguimanager.h"

#include "Modules/plugins/pluginmanager.h"

#include "opengl/openglgridpass.h"

#include "Modules/keyvalue/metatable_impl.h"

namespace Engine {
namespace Tools {

    SceneView::SceneView(Engine::Render::RenderWindow *renderer, const ImGuiManager &manager)
        : mManager(manager)
    {
        mCamera.setPosition({ 1, 1, 1 });

        mRenderTarget = renderer->createRenderTarget(&mCamera, { 100, 100 });

        std::unique_ptr<Render::RenderPass> pass;

        IF_PLUGIN(OpenGL)
        pass = std::make_unique<OpenGlGridPass>();
        else LOG_WARNING("No Grid RenderPass implemented!");

        if (pass)
            mRenderTarget->addPreRenderPass(std::move(pass));
    }

    SceneView::~SceneView()
    {
    }

    void SceneView::render()
    {
        ImGui::PushID(this);
        if (ImGui::Begin("SceneView")) {
            mRenderTarget->resize(ImGui::GetContentRegionAvail());
            mManager.render(*mRenderTarget);

            ImGuiIO &io = ImGui::GetIO();

            Vector2 dragDistance = io.MouseDelta;

            for (int i = 0; i < 2; ++i) {
                if (ImGui::IsItemHovered()) {

                    if (io.MouseClicked[i]) {
                        mMouseDown[i] = true;
                    }
                }

                if (mMouseDown[i]) {

                    if (dragDistance.length() >= io.MouseDragThreshold / 3.0f && !mDragging[i]) {
                        mDragging[i] = true;
                    }

                    if (io.MouseReleased[i]) {
                        mMouseDown[i] = false;
                        mDragging[i] = false;
                    }
                }
            }

            if (mDragging[0]) {
                mCamera.setPosition(mCamera.position() + mCamera.orientation() * Vector3 { -dragDistance.x / 20.0f, dragDistance.y / 20.0f, 0.0f });
            }

            mCamera.setPosition(mCamera.position() + mCamera.orientation() * Vector3 { Vector3::UNIT_Z } * io.MouseWheel / 5.0f);

            if (mDragging[1]) {

                mCamera.setOrientation(Quaternion { dragDistance.x / 200.0f, Vector3::UNIT_Y } * mCamera.orientation() * Quaternion { dragDistance.y / 200.0f, Vector3::UNIT_X });
            }
        }
        ImGui::End();
        ImGui::PopID();
    }

    Scene::Camera &SceneView::camera()
    {
        return mCamera;
    }

}
}

METATABLE_BEGIN(Engine::Tools::SceneView)
READONLY_PROPERTY(Camera, camera)
METATABLE_END(Engine::Tools::SceneView)