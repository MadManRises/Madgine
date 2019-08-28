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

    private:
        void renderSelection();
        void renderHierarchy();

    private:
        GUI::TopLevelWindow &mWindow;

		std::vector<SceneView> mSceneViews;

		bool mHierarchyVisible = false;
    };

}
}