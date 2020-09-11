#pragma once

#include "toolscollector.h"

#include "toolbase.h"

#include "sceneview.h"

#include "Madgine/scene/entity/entityptr.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

namespace Engine {
namespace Tools {

    struct SceneEditor : Tool<SceneEditor> {

        SERIALIZABLEUNIT;

        SceneEditor(ImRoot &root);
        SceneEditor(const SceneEditor &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        std::string_view key() const override;

        std::vector<SceneView> &views()
        {
            return mSceneViews;
        }

        int hoveredAxis() const;
        Scene::Entity::Transform *hoveredTransform() const;

        void deselect();
        void select(Render::Camera *camera);
        void select(const Scene::Entity::EntityPtr &entity);

        Scene::SceneManager &sceneMgr();

    private:
        void renderSelection();
        void renderHierarchy();
        void renderSettings();
        void renderHierarchyEntity(const Engine::Scene::Entity::EntityComponentPtr<Engine::Scene::Entity::Transform> &parentTransform);
        void renderEntity(Scene::Entity::EntityPtr &entity);
        void renderCamera(Render::Camera *camera);

    private:
        Window::MainWindow &mWindow;

        std::vector<SceneView> mSceneViews;

        Inspector *mInspector;
        Scene::SceneManager *mSceneMgr;

        Scene::Entity::EntityPtr mSelectedEntity;
        Render::Camera *mSelectedCamera = nullptr;

        bool mHierarchyVisible = false;
        bool mSettingsVisible = false;

        int mHoveredAxis = -1;
        Scene::Entity::Transform *mHoveredTransform = nullptr;

        //Settings
        Vector4 mBoneForward = { 1, 0, 0, 0 };
        float mDefaultBoneLength = 1.0f;
        bool mShowBoneNames = true;
    };

}
}

RegisterType(Engine::Tools::SceneEditor);