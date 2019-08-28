#include "../toolslib.h"

#include "sceneeditor.h"

#include "../imgui/imgui.h"
#include "../renderer/imguiroot.h"

#include "../renderer/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"


namespace Engine {
namespace Tools {

    SceneEditor::SceneEditor(ImGuiRoot &root)
        : Tool<SceneEditor>(root)
        , mWindow(root.window())
    {
    }

    bool SceneEditor::init()
    {
        mSceneViews.emplace_back(mWindow.getRenderer(), mRoot.manager());		
        return ToolBase::init();
    }

    void SceneEditor::render()
    {        
        if (mHierarchyVisible)
            renderHierarchy();
        renderSelection();
        for (SceneView &sceneView : mSceneViews) {
            sceneView.render();
		}
    }

    void SceneEditor::renderMenu()
    {
        if (ImGui::BeginMenu("Scene")) {
            
            ImGui::EndMenu();
        }

        if (mVisible) {

            if (ImGui::BeginMenu("SceneEditor")) {

                ImGui::MenuItem("Hierarchy", nullptr, &mHierarchyVisible);

                ImGui::EndMenu();
            }
        }
    }

    void SceneEditor::update()
    {

        ToolBase::update();
    }

    const char *SceneEditor::key() const
    {
        return "SceneEditor";
    }

    void SceneEditor::renderSelection()
    {
        if (ImGui::Begin("SceneEditor", &mVisible)) {

        }
        ImGui::End();
    }

    
    void SceneEditor::renderHierarchy()
    {
        if (ImGui::Begin("SceneEditor - Hierarchy", &mHierarchyVisible)) {
            
        }
        ImGui::End();
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

RegisterType(Engine::Tools::SceneEditor);