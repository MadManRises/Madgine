#pragma once

#include "../toolscollector.h"

#include "sceneview.h"

namespace Engine {
namespace Tools {

    struct SceneEditor : public Tool<SceneEditor> {

        SERIALIZABLEUNIT;

        SceneEditor(ImGuiRoot &root);
        SceneEditor(const SceneEditor &) = delete;

        virtual bool init() override;

        virtual void render() override;
        virtual void renderMenu() override;
        virtual void update() override;

        const char *key() const override;

        std::vector<SceneView> &views()
        {
            return mSceneViews;
        }

		int hoveredAxis() const;
        Scene::Entity::Transform *hoveredTransform() const;

    private:
        void renderSelection();
        void renderHierarchy();
        void renderEntity(Scene::Entity::Entity *entity);
        void renderCamera(Scene::Camera *camera);

    private:
        GUI::TopLevelWindow &mWindow;

        std::vector<SceneView> mSceneViews;

        Inspector *mInspector;
        Scene::SceneManager *mSceneMgr;

        Scene::Entity::Entity *mSelectedEntity = nullptr;
        Scene::Camera *mSelectedCamera = nullptr;

        bool mHierarchyVisible = false;

		int mHoveredAxis = -1;
        Scene::Entity::Transform *mHoveredTransform = nullptr;
    };

}
}