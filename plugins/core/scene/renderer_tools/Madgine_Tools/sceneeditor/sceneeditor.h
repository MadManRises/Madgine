#pragma once

#include "toolscollector.h"

#include "toolbase.h"

#include "sceneview.h"

#include "Madgine/scene/entity/entityptr.h"

#include "Madgine/scene/entity/entitycomponentptr.h"

namespace Engine {
namespace Tools {

    struct SceneEditor : Tool<SceneEditor> {

        SERIALIZABLEUNIT(SceneEditor);

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
        const Engine::Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> &hoveredTransform() const;

        void deselect();
        void select(Render::Camera *camera);
        void select(const Scene::Entity::EntityPtr &entity);

        Scene::SceneManager &sceneMgr();

        bool render3DCursor() const;

    private:
        void renderSelection();
        void renderHierarchy();
        void renderSettings();
        void renderEntity(Scene::Entity::EntityPtr &entity);
        void renderCamera(Render::Camera *camera);

        void handleInputs();

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
        Engine::Scene::Entity::EntityComponentPtr<Scene::Entity::Transform> mHoveredTransform;

        //Entity-Cache
        struct EntityNode {
            Scene::Entity::EntityPtr mEntity;
            std::list<EntityNode> mChildren;
        };
        std::list<EntityNode> mEntityCache;
        std::map<Scene::Entity::EntityPtr, EntityNode *> mEntityMapping;

        void updateEntityCache();
        bool updateEntityCache(EntityNode &node, const Scene::Entity::EntityPtr &parent = {});
        void createEntityMapping(Scene::Entity::EntityPtr e);
        void renderHierarchyEntity(EntityNode &entity);
        void eraseNode(EntityNode &node);

        //Settings
        Vector4 mBoneForward = { 1, 0, 0, 0 };
        float mDefaultBoneLength = 1.0f;
        bool mShowBoneNames = true;
        bool mRender3DCursor = false;
    };

}
}

RegisterType(Engine::Tools::SceneEditor);