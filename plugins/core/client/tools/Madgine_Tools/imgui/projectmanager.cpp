#include "../clienttoolslib.h"
#include "serialize/filesystem/filesystemlib.h"

#include "projectmanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/resources/resourcemanager.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "imgui/imgui_internal.h"

#include "Madgine/window/mainwindow.h"
#include "clientimroot.h"

#include "Meta/serialize/formatter/xmlformatter.h"

#include "serialize/filesystem/filemanager.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/hierarchy/statetransmissionflags.h"

METATABLE_BEGIN_BASE(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::ProjectManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
#if ENABLE_PLUGINS
ENCAPSULATED_FIELD(ProjectRoot, projectRoot, setProjectRoot)
ENCAPSULATED_FIELD(Config, config, setConfig)
#endif
SERIALIZETABLE_END(Engine::Tools::ProjectManager)

UNIQUECOMPONENT(Engine::Tools::ProjectManager)

namespace Engine {
namespace Tools {

    ProjectManager::ProjectManager(ImRoot &root)
        : Tool<ProjectManager>(root)
    {
    }

    Threading::Task<bool> ProjectManager::init()
    {
        mWindow = &static_cast<const ClientImRoot &>(mRoot).window();

#if ENABLE_PLUGINS
        mWindow->taskQueue()->queue([this]() {
            load();
        });
#endif

        co_return co_await ToolBase::init();
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

        bool openProjectDialog = false;
        bool openConfigDialog = false;

        if (ImGui::BeginMenu("Project")) {

            if (ImGui::MenuItem("New Project...")) {
                throw 0;
            }

            if (ImGui::MenuItem("Open Project...")) {
                openProjectDialog = true;
            }

            ImGui::Separator();

            if (mProjectRoot.empty())
                ImGui::PushDisabled();
            if (ImGui::MenuItem("New Config...")) {
                openConfigDialog = true;
            }
            if (ImGui::MenuItem("Save Config")) {
                save();
            }

            ImGui::Separator();

            for (const std::string &config : projectConfigs()) {
                if (ImGui::MenuItem(config.c_str(), nullptr, mConfig == config)) {
                    setConfig(config);
                }
            }
            if (mProjectRoot.empty())
                ImGui::PopDisabled();
            ImGui::Separator();
            if (ImGui::MenuItem("Quit")) {
                throw 0;
            }
            ImGui::EndMenu();
        }

        if (openProjectDialog) {
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

            load();

            if (!mProjectRoot.empty()) {
                Resources::ResourceManager::getSingleton().registerResourceLocation(mProjectRoot / "data", 80);
            }
        }
    }

    std::vector<std::string> ProjectManager::projectConfigs() const
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
            load();
        }
    }
#endif

    const Filesystem::Path &ProjectManager::projectRoot() const
    {
        return mProjectRoot;
    }

    const std::string &ProjectManager::config() const
    {
        return mConfig;
    }

    void ProjectManager::save()
    {
        Filesystem::Path filePath = mProjectRoot / "data" / (mConfig + ".layout");

        Filesystem::FileManager file("Layout");
        Serialize::FormattedSerializeStream out = file.openWrite(filePath, std::make_unique<Serialize::XMLFormatter>());

        Serialize::write(out, *mWindow, "Layout");
    }

    void ProjectManager::load()
    {
        if (!mConfig.empty()) {

            Filesystem::Path filePath = mProjectRoot / "data" / (mConfig + ".layout");

            Filesystem::FileManager file("Layout");
            Serialize::FormattedSerializeStream in = file.openRead(filePath, std::make_unique<Serialize::XMLFormatter>());
            if (in) {
                Serialize::StreamResult result = Serialize::read(in, *mWindow, nullptr, {}, Serialize::StateTransmissionFlags_ApplyMap | Serialize::StateTransmissionFlags_Activation | Serialize::StateTransmissionFlags_SkipId);
                if (result.mState != Serialize::StreamState::OK) {
                    LOG_ERROR("Failed loading '" << filePath << "' with following Error: "
                                                 << "\n"
                                                 << result);
                }
            } else {
                LOG_ERROR("File not found '" << filePath << "'!");
            }
        }
    }

}
}
