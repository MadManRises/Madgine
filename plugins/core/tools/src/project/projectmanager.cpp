#include "../toolslib.h"

#include "projectmanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/resources/resourcemanager.h"

METATABLE_BEGIN(Engine::Tools::ProjectManager)
METATABLE_END(Engine::Tools::ProjectManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
FIELD(mProjectRootString)
SERIALIZETABLE_END(Engine::Tools::ProjectManager)

UNIQUECOMPONENT(Engine::Tools::ProjectManager)

RegisterType(Engine::Tools::ProjectManager)

    namespace Engine
{
    namespace Tools {

        ProjectManager::ProjectManager(ImRoot &root)
            : Tool<ProjectManager>(root)
        {
        }

        const char *ProjectManager::key() const
        {
            return "ProjectManager";
        }

        void ProjectManager::renderMenu()
        {
            if (mProjectRootString != mProjectRoot.str()) {
                setProjectRoot(mProjectRootString);
            }

            ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);
            if (ImGui::BeginPopupModal("OpenFolder")) {

                if (ImGui::Button("Up")) {
                    mCurrentSelectionPath = mCurrentPath;
                    mCurrentPath = mCurrentPath / "..";
                }

                ImGui::SameLine();

                if (ImGui::BeginCombo("Current", mCurrentPath.c_str())) {

                    ImGui::EndCombo();
                }

                if (ImGui::BeginChild("CurrentFolder", { 0.0f, -ImGui::GetItemsLineHeightWithSpacing() })) {

                    for (Filesystem::FileQueryResult result : Filesystem::listFolders(mCurrentPath)) {

                        bool selected = mCurrentSelectionPath == result.path();

                        if (ImGui::Selectable(result.path().filename().c_str(), selected)) {
                            mCurrentSelectionPath = result.path();
                        }

                        if (result.isDir()) {
                            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                                mCurrentPath = result.path();
                            }
                        }
                    }

                    ImGui::EndChild();
                }

                bool hasValidPath = mCurrentSelectionPath != mCurrentPath;

                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (!hasValidPath)
                    ImGui::PushDisabled();
                if (ImGui::Button("Open")) {
                    setProjectRoot(mCurrentSelectionPath);
                }
                if (!hasValidPath)
                    ImGui::PopDisabled();
                ImGui::EndPopup();
            }

            bool openFolderDialog = false;

            if (ImGui::BeginMenu("Project")) {

                if (ImGui::MenuItem("Open Folder...")) {
                    openFolderDialog = true;
                }
                ImGui::EndMenu();
            }

            if (openFolderDialog == true) {
                if (mProjectRoot.empty()) {
                    mCurrentPath = Filesystem::Path { "." }.absolute();
                    mCurrentSelectionPath = {};
                } else {
                    mCurrentSelectionPath = mProjectRoot.absolute();
                    mCurrentPath = mCurrentSelectionPath.parentPath();
                }
                ImGui::OpenPopup("OpenFolder");
            }
        }

        void ProjectManager::setProjectRoot(const Filesystem::Path &root)
        {
            if (mProjectRoot != root) {

                if (!mProjectRoot.empty()) {

                    //Resources::ResourceManager::getSingleton().unregisterResourceLocation(mProjectRoot);
                }

                mProjectRoot = root;
                mProjectRootString = mProjectRoot.str();
                mProjectRootChanged.emit(mProjectRootString);

                if (!mProjectRoot.empty()) {

                    Resources::ResourceManager::getSingleton().registerResourceLocation(mProjectRoot / "data", 80);
                }
            }
        }

        const Filesystem::Path Engine::Tools::ProjectManager::projectRoot() const
        {
            return mProjectRoot;
        }

    }
}