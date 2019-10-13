#include "../opengltoolslib.h"

#include "sceneview.h"

#include "renderer/imroot.h"
#include "imgui/imgui.h"

#include "imgui/imguiaddons.h"

#include "Madgine/render/renderwindow.h"

#include "Madgine_Tools/imgui/immanager.h"

#include "Modules/plugins/pluginmanager.h"

#include "opengl/openglgridpass.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine_Tools/im3d/im3drenderpass.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "sceneeditor.h"

#include "Madgine/scene/entity/components/transform.h"

#include "Modules/math/plane.h"

#include "Modules/math/geometry3.h"

#include "OpenGL/openglmeshloader.h"

#include "Madgine/app/application.h"

#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/entity.h"

#include "OpenGL/openglmesh.h"

#include "inspector/inspector.h"

#include "OpenGL/openglmeshdata.h"

namespace Engine {
namespace Tools {

    static Plane cameraPlane(const Scene::Camera &camera, const Vector3 &point, const Vector3 *axis1 = nullptr, const Vector3 *axis2 = nullptr)
    {

        const Ray &ray = camera.toRay();

        Vector3 normal;

        if (axis1) {
            Vector3 helper;
            if (axis2) {
                helper = *axis2;
            } else {
                helper = axis1->crossProduct(ray.mDir);
            }
            normal = helper.crossProduct(*axis1);
        } else {
            normal = ray.mDir;
        }

        return { point, normal };
    }

    SceneView::SceneView(SceneEditor *editor, Engine::Render::RenderWindow *renderer, const ImManager &manager)
        : mManager(manager)
        , mEditor(editor)
    {
        mCamera.mPosition = { 0, 0.5, -1 };

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
        ImGui::SetNextWindowSizeConstraints({ 100, 100 }, { 1000000, 1000000 });
        if (ImGui::Begin("SceneView")) {
            mRenderTarget->resize(ImGui::GetContentRegionAvail());
            bool pressed = mManager.render(*mRenderTarget);
			if (pressed && !mDragging[0])
                if (!Im3D::IsAnyObjectHovered())
                    mEditor->deselect();

            ImGuiIO &io = ImGui::GetIO();
            Im3DIO &io3D = Im3D::GetIO();

            Vector2 dragDistance = io.MouseDelta;

            const Ray &ray = Im3D::GetCurrentContext()->mMouseRay;

            constexpr Vector3 axes[3] = {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 }
            };

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {

                if (io.MouseClicked[0] && mEditor->hoveredAxis() >= 0) {
                    mDraggedAxis = mEditor->hoveredAxis();
                    mDragStartRay = ray;
                    mDragStoredPosition = mEditor->hoveredTransform()->getPosition();

                    Vector3 axis = axes[mDraggedAxis];

                    Plane plane = cameraPlane(mCamera, mDragStoredPosition, &axis);

                    if (auto intersection = Intersect(plane, mDragStartRay)) {

                        mMouseDown[0] = true;
                        mDragTransform = mEditor->hoveredTransform();
                        mDragStoredMatrix = mEditor->hoveredTransform()->matrix();
                        mDragRelMousePosition = mDragStartRay.point(intersection[0]) - mDragStoredPosition;
                    }
                }

                for (int i = 1; i < 3; ++i) {
                    if (io.MouseClicked[i]) {
                        mMouseDown[i] = true;
                    }
                }

                Vector2 pos = ImGui::GetItemRectMin();
                Vector2 size = ImGui::GetItemRectSize();

                io3D.mNextFrameMouseRay = mCamera.mousePointToRay(Vector2 { io.MousePos } - pos, size);

                mCamera.mPosition += mCamera.mOrientation * Vector3 { Vector3::UNIT_Z } * io.MouseWheel / 5.0f;
            }

            for (int i = 0; i < 3; ++i) {
                mMouseClicked[i] = false;
                if (mMouseDown[i]) {

                    if (dragDistance.length() >= io.MouseDragThreshold / 3.0f && !mDragging[0] && !mDragging[1] && !mDragging[2]) {
                        mDragging[i] = true;
                    }

                    if (io.MouseReleased[i]) {
                        mMouseDown[i] = false;
                        if (!mDragging[i]) {
                            mMouseClicked[i] = true;
                        }
                        mDragging[i] = false;
                    }
                }
            }

            if (mDragging[2]) {
                mCamera.mPosition += mCamera.mOrientation * Vector3 { -dragDistance.x / 50.0f, dragDistance.y / 50.0f, 0.0f };
            }

            if (mDragging[1]) {
                mCamera.mOrientation = Quaternion { dragDistance.x / 200.0f, Vector3::UNIT_Y } * mCamera.mOrientation * Quaternion { dragDistance.y / 200.0f, Vector3::UNIT_X };
            }

            if (mDragging[0]) {

                Vector3 axis = axes[mDraggedAxis];

                Plane targetPlane = cameraPlane(mCamera, mDragStoredPosition, &axis);

                if (auto intersection = Intersect(targetPlane, ray)) {

                    Vector3 distance = ray.point(intersection[0]) - mDragStoredPosition - mDragRelMousePosition;

                    if (mDraggedAxis != 0)
                        distance.x = 0.0f;
                    if (mDraggedAxis != 1)
                        distance.y = 0.0f;
                    if (mDraggedAxis != 2)
                        distance.z = 0.0f;

                    mDragTransform->setPosition(mDragStoredPosition + distance);
                }
            }

            if (ImGui::BeginDragDropTarget()) {
                Vector3 pos = ray.point(5.0f);
                Render::OpenGLMeshLoader::ResourceType *resource;
                if (ImGui::AcceptDraggableValueType(resource)) {
                    Scene::Entity::Entity *e = App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().createEntity();
                    e->addComponent<Scene::Entity::Transform>()->setPosition(pos);
                    e->addComponent<Render::OpenGLMesh>()->set(resource);
                    mEditor->select(e);
                } else if (ImGui::IsDraggableValueTypeBeingAccepted(resource)) {
                    resource->setPersistent(true);
                    std::shared_ptr<Render::OpenGLMeshData> data = resource->loadData();
                    Im3D::NativeMesh(data.get(), data->mAABB, Matrix4::TranslationMatrix(pos));
                }
                ImGui::EndDragDropTarget();
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