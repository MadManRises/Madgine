#include "../toolslib.h"
#include "OpenGL/opengllib.h"

#include "sceneview.h"

#include "../renderer/imguiroot.h"
#include "imgui/imgui.h"

#include "imgui/imguiaddons.h"

#include "Madgine/render/renderwindow.h"

#include "../renderer/imguimanager.h"

#include "Modules/plugins/pluginmanager.h"

#include "opengl/openglgridpass.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "im3d/im3drenderpass.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "sceneeditor.h"

#include "Madgine/scene/entity/components/transform.h"

#include "Modules/math/plane.h"

#include "Modules/math/geometry.h"

namespace Engine {
namespace Tools {

    SceneView::SceneView(SceneEditor *editor, Engine::Render::RenderWindow *renderer, const ImGuiManager &manager)
        : mManager(manager)
        , mEditor(editor)
    {
        mCamera.setPosition({ 0, 0.5, -1 });

        mRenderTarget = renderer->createRenderTarget(&mCamera, { 100, 100 });

        std::unique_ptr<Render::RenderPass> pass;

        IF_PLUGIN(OpenGL)
        pass = std::make_unique<OpenGlGridPass>();
        else LOG_WARNING("No Grid RenderPass implemented!");

        if (pass)
            mRenderTarget->addPostRenderPass(std::move(pass));

        mRenderTarget->addPostRenderPass(std::make_unique<Render::Im3DRenderPass>());
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
            Im3DIO &io3D = Im3D::GetIO();

            Vector2 dragDistance = io.MouseDelta;

            if (ImGui::IsItemHovered()) {

                if (io.MouseClicked[0] && mEditor->hoveredAxis() >= 0) {
                    mMouseDown[0] = true;
                    mDraggedAxis = mEditor->hoveredAxis();
                    mDragStartRay = Im3D::GetCurrentContext()->mMouseRay;
                    mDragTransform = mEditor->hoveredTransform();
                    mDragStoredMatrix = mEditor->hoveredTransform()->matrix();
                    mDragStoredPosition = mEditor->hoveredTransform()->getPosition();
                }

                for (int i = 1; i < 3; ++i) {
                    if (io.MouseClicked[i]) {
                        mMouseDown[i] = true;
                    }
                }

                Vector2 pos = ImGui::GetItemRectMin();
                Vector2 size = ImGui::GetItemRectSize();

                Im3D::GetIO().mNextFrameMouseRay = mCamera.mousePointToRay(Vector2 { io.MousePos } - pos, size);
            }

            for (int i = 0; i < 3; ++i) {
                if (mMouseDown[i]) {

                    if (dragDistance.length() >= io.MouseDragThreshold / 3.0f && !mDragging[0] && !mDragging[1] && !mDragging[2]) {
                        mDragging[i] = true;
                    }

                    if (io.MouseReleased[i]) {
                        mMouseDown[i] = false;
                        mDragging[i] = false;
                    }
                }
            }

            if (mDragging[2]) {
                mCamera.setPosition(mCamera.position() + mCamera.orientation() * Vector3 { -dragDistance.x / 50.0f, dragDistance.y / 50.0f, 0.0f });
            }

            mCamera.setPosition(mCamera.position() + mCamera.orientation() * Vector3 { Vector3::UNIT_Z } * io.MouseWheel / 5.0f);

            if (mDragging[1]) {
                mCamera.setOrientation(Quaternion { dragDistance.x / 200.0f, Vector3::UNIT_Y } * mCamera.orientation() * Quaternion { dragDistance.y / 200.0f, Vector3::UNIT_X });
            }

            if (mDragging[0]) {

                const Ray &cameraRay = mCamera.toRay();

                constexpr Vector3 axes[3] = {
                    { 1, 0, 0 },
                    { 0, 1, 0 },
                    { 0, 0, 1 }
                };

                Vector3 axis = axes[mDraggedAxis];

                Vector3 helper = axis.crossProduct(cameraRay.mDir);

                Plane targetPlane { mDragStoredPosition, helper.crossProduct(axis) };

                const Ray &ray = Im3D::GetCurrentContext()->mMouseRay;

                float rayParam;

                if (Intersect(targetPlane, ray, &rayParam)) {

                    Vector3 distance = ray.mPoint + rayParam * ray.mDir - mDragStoredPosition;

                    if (mDraggedAxis != 0)
                        distance.x = 0.0f;
                    if (mDraggedAxis != 1)
                        distance.y = 0.0f;
                    if (mDraggedAxis != 2)
                        distance.z = 0.0f;

                    mDragTransform->setPosition(mDragStoredPosition + distance);
                }
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