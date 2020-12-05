#include "../toolslib.h"

#include "projectmanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Interfaces/filesystem/api.h"

#include "Modules/resources/resourcemanager.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

METATABLE_BEGIN(Engine::Tools::ProjectManager)
METATABLE_END(Engine::Tools::ProjectManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
#if ENABLE_PLUGINS
ENCAPSULATED_FIELD(ProjectRoot, projectRoot, setProjectRoot)
ENCAPSULATED_FIELD(Config, config, setConfig)
#endif
SERIALIZETABLE_END(Engine::Tools::ProjectManager)

UNIQUECOMPONENT(Engine::Tools::ProjectManager)



    namespace Engine
{
    namespace Tools {

        ProjectManager::ProjectManager(ImRoot &root)
            : Tool<ProjectManager>(root)
        {
        }

        std::string_view ProjectManager::key() const
        {
            return "ProjectManager";
        }

        void ProjectManager::renderMenu()
        {
#if ENABLE_PLUGINS
            ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);
            if (ImGui::BeginPopupModal("OpenFolder")) {

                bool accepted;
                if (ImGui::DirectoryPicker(&mCurrentPath, &mCurrentSelectionPath, accepted)) {
                    if (accepted)
                        setProjectRoot(mCurrentSelectionPath);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            static std::string configNameBuffer;
            if (ImGui::BeginPopupModal("NewConfig")) {

                ImGui::InputText("Name", &configNameBuffer);
                if (configNameBuffer.empty()) {
                    ImGui::PushDisabled();
                }
                if (ImGui::Button("Ok")) {
                    setConfig(configNameBuffer);

                    ImGui::CloseCurrentPopup();
                }
                if (configNameBuffer.empty()) {
                    ImGui::PopDisabled();
                }
                ImGui::SameLine();
                if (ImGui::Button("Abort"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            bool openFolderDialog = false;
            bool openConfigDialog = false;

            if (ImGui::BeginMenu("Project")) {

                if (ImGui::MenuItem("Open Folder...")) {
                    openFolderDialog = true;
                }
                if (mProjectRoot.empty())
                    ImGui::PushDisabled();
                if (ImGui::MenuItem("New Config...")) {
                    openConfigDialog = true;
                }
                if (mProjectRoot.empty())
                    ImGui::PopDisabled();
                else {
                    ImGui::Separator();
                    for (const std::string &config : projectConfigs()) {
                        if (ImGui::MenuItem(config.c_str(), nullptr, mConfig == config)) {
                            setConfig(config);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            if (openFolderDialog) {
                if (mProjectRoot.empty()) {
                    mCurrentPath = Filesystem::Path { "." }.absolute();
                    mCurrentSelectionPath.clear();
                } else {
                    mCurrentSelectionPath = mProjectRoot.absolute();
                    mCurrentPath = mCurrentSelectionPath.parentPath();
                }
                ImGui::OpenPopup("OpenFolder");
            }
            if (openConfigDialog) {
                configNameBuffer.clear();
                ImGui::OpenPopup("NewConfig");
            }
#endif
        }

#if ENABLE_PLUGINS
        void ProjectManager::setProjectRoot(const Filesystem::Path &root)
        {
            if (mProjectRoot != root) {

                if (!mProjectRoot.empty()) {

                    //Resources::ResourceManager::getSingleton().unregisterResourceLocation(mProjectRoot);
                }

                mProjectRoot = root;
                const std::vector<std::string> &configs = projectConfigs();
                mConfig = configs.empty() ? "" : configs.front();

                

                mProjectChanged.emit(mProjectRoot, mConfig);

                if (!mProjectRoot.empty()) {

                    Resources::ResourceManager::getSingleton().registerResourceLocation(mProjectRoot / "data", 80);
                }
            }
        }

        std::vector<std::string> Engine::Tools::ProjectManager::projectConfigs() const
        {
            if (mProjectRoot.empty())
                return {};
            std::vector<std::string> result;
            for (const Filesystem::Path &p : Filesystem::listFiles(mProjectRoot)) {
                if (p.extension() == ".cfg" && p.str().find('(') == std::string::npos) {
                    result.push_back(std::string { p.stem() });
                }
            }
            return result;
        }

        void ProjectManager::setConfig(const std::string &config)
        {
            assert(!mProjectRoot.empty() || config.empty());
            if (mConfig != config) {
                mConfig = config;
                mProjectChanged.emit(mProjectRoot, mConfig);
            }
        }
#endif

        const Filesystem::Path &Engine::Tools::ProjectManager::projectRoot() const
        {
            return mProjectRoot;
        }

        const std::string &Engine::Tools::ProjectManager::config() const
        {
            return mConfig;
        }

    }
}
