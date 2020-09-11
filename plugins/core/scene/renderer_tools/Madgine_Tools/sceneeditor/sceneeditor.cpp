#include "../scenerenderertoolslib.h"

#include "sceneeditor.h"

#include "Madgine_Tools/imgui/clientimroot.h"
#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/entity/entitycomponentcollector.h"
#include "Madgine/scene/scenemanager.h"

#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"

#include "inspector/inspector.h"

#include "im3d/im3d.h"

#include "Modules/resources/resourcemanager.h"

#include "Modules/math/boundingbox.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/scene/entity/entity.h"

namespace Engine {
namespace Tools {

    SceneEditor::SceneEditor(ImRoot &root)
        : Tool<SceneEditor>(root)
        , mWindow(static_cast<const ClientImRoot &>(*root.parent()).window())
    {
    }

    bool SceneEditor::init()
    {
        mSceneMgr = &App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>();
        mInspector = &mRoot.getTool<Inspector>();
        mSceneViews.emplace_back(this, mWindow.getRenderer());

        return ToolBase::init();
    }

    void SceneEditor::finalize()
    {
        mSceneViews.clear();

        ToolBase::finalize();
    }

    void SceneEditor::render()
    {
        Engine::Threading::DataLock lock(mSceneMgr->mutex(), Engine::Threading::AccessMode::READ);
        if (mHierarchyVisible)
            renderHierarchy();
        if (mSettingsVisible)
            renderSettings();
        renderSelection();
        for (SceneView &sceneView : mSceneViews) {
            sceneView.render();
        }
    }

    void SceneEditor::renderMenu()
    {
        if (mVisible) {

            if (ImGui::BeginMenu("SceneEditor")) {

                ImGui::MenuItem("Hierarchy", nullptr, &mHierarchyVisible);

                ImGui::MenuItem("Settings", nullptr, &mSettingsVisible);

                ImGui::EndMenu();
            }
        }
    }

    std::string_view SceneEditor::key() const
    {
        return "SceneEditor";
    }

    int SceneEditor::hoveredAxis() const
    {
        return mHoveredAxis;
    }

    Scene::Entity::Transform *SceneEditor::hoveredTransform() const
    {
        return mHoveredTransform;
    }

    void SceneEditor::deselect()
    {
        mSelectedEntity = {};
        mSelectedCamera = nullptr;
        mHoveredAxis = -1;
    }

    void SceneEditor::select(Render::Camera *camera)
    {
        mSelectedEntity = {};
        mSelectedCamera = camera;
        mHoveredAxis = -1;
    }

    void SceneEditor::select(const Scene::Entity::EntityPtr &entity)
    {
        mSelectedEntity = entity;
        mSelectedCamera = nullptr;
        mHoveredAxis = -1;
    }

    Scene::SceneManager &SceneEditor::sceneMgr()
    {
        return *mSceneMgr;
    }

    void SceneEditor::renderSelection()
    {
        if (ImGui::Begin("SceneEditor", &mVisible)) {
            if (mSelectedEntity)
                renderEntity(mSelectedEntity);
            if (mSelectedCamera)
                renderCamera(mSelectedCamera);
        }
        ImGui::End();
    }

    void SceneEditor::renderHierarchy()
    {
        if (ImGui::Begin("SceneEditor - Hierarchy", &mHierarchyVisible)) {

            renderHierarchyEntity({});

            if (ImGui::Button("+ New Entity")) {
                select(mSceneMgr->createEntity());
            }

            ImGui::Separator();

            /*for (Scene::Camera &camera : mSceneMgr->cameras()) {
                std::string name = camera.mName;
                if (name.empty())
                    name = "<unnamed camera>";
                if (ImGui::Selectable(name.c_str(), mSelectedCamera == &camera)) {
                    select(&camera);
                }
                ImGui::DraggableValueTypeSource(name, this, ValueType { camera });
            }

            if (ImGui::Button("+ New Camera")) {
                select(mSceneMgr->createCamera());
            }*/
        }
        ImGui::End();
    }

    void SceneEditor::renderSettings()
    {
        if (ImGui::Begin("SceneEditor - Settings", &mSettingsVisible)) {

            ImGui::ValueTypeDrawer { "Bone-Forward" }.draw(mBoneForward);
            ImGui::DragFloat("Default Bone Length", &mDefaultBoneLength);
            ImGui::Checkbox("Show Bone Names", &mShowBoneNames);
        }
        ImGui::End();
    }

    void SceneEditor::renderHierarchyEntity(const Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::Transform> &parentTransform)
    {
        for (Engine::Scene::Entity::EntityPtr e = mSceneMgr->entities().begin(); e != mSceneMgr->entities().end(); ++e) {
            Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::Transform> transform = e.getComponent<Scene::Entity::Transform>();
            if ((!transform && parentTransform) || (transform && parentTransform != transform->parent()))
                continue;

            const char *name = e->key().c_str();
            if (!name[0])
                name = "<unnamed>";

            bool hovered = mSelectedEntity == e;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
            if (hovered)
                flags |= ImGuiTreeNodeFlags_Selected;

            bool hasChildren = false;
            if (transform) {
                for (Engine::Scene::Entity::EntityPtr child = mSceneMgr->entities().begin(); child != mSceneMgr->entities().end(); ++child) {
                    Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::Transform> childTransform = child.getComponent<Scene::Entity::Transform>();
                    if (childTransform && transform == childTransform->parent()) {
                        hasChildren = true;
                        break;
                    }
                }
            }
            if (!hasChildren)
                flags |= ImGuiTreeNodeFlags_Leaf;

            bool open = ImGui::TreeNodeEx(name, flags);

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
                select(e);
            }
            if (transform) {
                ImGui::DraggableValueTypeSource(name, this, ValueType { *e });
                if (ImGui::BeginDragDropTarget()) {
                    Engine::Scene::Entity::Entity *newChild;
                    if (ImGui::AcceptDraggableValueType(newChild)) {
                        Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::Transform> childTransform = mSceneMgr->toEntityPtr(newChild).getComponent<Engine::Scene::Entity::Transform>();
                        if (childTransform)
                            childTransform->setParent(transform);
                    }
                    ImGui::EndDragDropTarget();
                }

                Engine::Matrix4 transformM = transform->worldMatrix(mSceneMgr->entityComponentList<Engine::Scene::Entity::Transform>());
                Engine::AABB bb = { { -0.2f, -0.2f, -0.2f }, { 0.2f, 0.2f, 0.2f } };
                if (e->hasComponent<Scene::Entity::Mesh>() && e.getComponent<Scene::Entity::Mesh>()->data())
                    bb = e.getComponent<Scene::Entity::Mesh>()->aabb();

                Im3DBoundingObjectFlags flags = Im3DBoundingObjectFlags_ShowOnHover;
                if (hovered)
                    flags |= Im3DBoundingObjectFlags_ShowOutline;

                if (Im3D::BoundingBox(name, bb, transformM, flags)) {
                    if (ImGui::IsMouseClicked(0)) {
                        select(e);
                    }
                    hovered = true;
                }
            }
            if (open) {
                if (transform)
                    renderHierarchyEntity(transform);
                ImGui::TreePop();
            }
        }
    }

    void SceneEditor::renderEntity(Scene::Entity::EntityPtr &entity)
    {
        bool found = false;
        for (Scene::Entity::Entity &e : mSceneMgr->entities()) {
            if (&e == entity) {
                found = true;
                break;
            }
        }
        if (!found) {
            entity = {};
            return;
        }

        ImGui::InputText("Name", &entity->mName);

        for (Scene::Entity::EntityComponentBase *component : entity->components()) {
            std::string name = std::string { component->key() };
            bool open = ImGui::TreeNode(name.c_str());
            ImGui::DraggableValueTypeSource(name.c_str(), this, ValueType { component });
            if (open) {
                mInspector->draw(component);
                ImGui::TreePop();
            }
        }

        if (ImGui::Button("+ Add Component")) {
            ImGui::OpenPopup("add_component_popup");
        }

        if (ImGui::BeginPopup("add_component_popup")) {
            for (const std::pair<const std::string_view, IndexRef> &componentDesc : Scene::Entity::sComponentsByName()) {
                if (componentDesc.second.isValid() && !entity->hasComponent(componentDesc.first)) {
                    if (ImGui::Selectable(componentDesc.first.data())) {
                        entity.addComponent(componentDesc.first);
                        if (componentDesc.first == "Transform") {
                            entity.getComponent<Scene::Entity::Transform>()->setPosition({ 0, 0, 0 });
                            entity.getComponent<Scene::Entity::Transform>()->setScale({ 0.0001f, 0.0001f, 0.0001f });
                        }
                        if (componentDesc.first == "Mesh") {
                            entity.getComponent<Scene::Entity::Mesh>()->setName("mage_animated");
                        }
                        if (componentDesc.first == "Skeleton") {
                            entity.getComponent<Scene::Entity::Skeleton>()->setName("mage_animated");
                        }
                        if (componentDesc.first == "Animation") {
                            entity.getComponent<Scene::Entity::Animation>()->setName("mage_animated");
                            entity.getComponent<Scene::Entity::Animation>()->setCurrentAnimationName("Walk");
                        }
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }

        if (Scene::Entity::Transform *t = entity.getComponent<Scene::Entity::Transform>()) {
            constexpr Vector4 colors[] = {
                { 0.5f, 0, 0, 0.5f },
                { 0, 0.5f, 0, 0.5f },
                { 0, 0, 0.5f, 0.5f }
            };
            constexpr Vector3 offsets[] = {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 }
            };

            const char *labels[] = {
                "x-move",
                "y-move",
                "z-move"
            };

            mHoveredAxis = -1;
            mHoveredTransform = nullptr;

            Vector3 pos = (t->worldMatrix(entity.sceneMgr()->entityComponentList<Scene::Entity::Transform>()) * Vector4::UNIT_W).xyz();

            for (size_t i = 0; i < 3; ++i) {
                Im3D::Arrow(IM3D_TRIANGLES, 0.1f, pos, pos + offsets[i], colors[i]);
                if (Im3D::BoundingBox(labels[i], 0, 2)) {
                    mHoveredAxis = i;
                    mHoveredTransform = t;
                }
            }

            if (Scene::Entity::Skeleton *s = entity.getComponent<Scene::Entity::Skeleton>()) {
                if (Render::SkeletonDescriptor *skeleton = s->data()) {
                    for (size_t i = 0; i < skeleton->mBones.size(); ++i) {
                        const Engine::Render::Bone &bone = skeleton->mBones[i];

                        Matrix4 m = s->matrices()[i] * bone.mOffsetMatrix.Inverse() * skeleton->mMatrix.Inverse();
                        Matrix4 world = t->worldMatrix(entity.sceneMgr()->entityComponentList<Scene::Entity::Transform>());

                        if (mShowBoneNames)
                            Im3D::Text(bone.mName.c_str(), world * m, 2.0f);

                        Vector4 start = world * m * Vector4::UNIT_W;
                        Vector4 end;

                        if (bone.mFirstChild != std::numeric_limits<uint32_t>::max()) {
                            Matrix4 m_child = s->matrices()[bone.mFirstChild] * skeleton->mBones[bone.mFirstChild].mOffsetMatrix.Inverse() * skeleton->mMatrix.Inverse();
                            end = world * m_child * Vector4::UNIT_W;
                        } else {
                            end = world * m * skeleton->mMatrix * (mBoneForward * mDefaultBoneLength) + (1.0f - mBoneForward.w) * start;                  
                        }
                        float length = (end - start).xyz().length();
                        Im3D::Arrow(IM3D_LINES, 0.1f * length, start.xyz(), end.xyz());
                    }
                }
            }
        }
    }

    void SceneEditor::renderCamera(Render::Camera *camera)
    {
        mInspector->draw(camera);
    }

}
}

UNIQUECOMPONENT(Engine::Tools::SceneEditor);

METATABLE_BEGIN(Engine::Tools::SceneEditor)
READONLY_PROPERTY(Views, views)
METATABLE_END(Engine::Tools::SceneEditor)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::SceneEditor, Engine::Tools::ToolBase)
FIELD(mHierarchyVisible)
SERIALIZETABLE_END(Engine::Tools::SceneEditor)
