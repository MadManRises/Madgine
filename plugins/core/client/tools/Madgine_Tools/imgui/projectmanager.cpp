#include "../clienttoolslib.h"
#include "Madgine/serialize/filesystem/filesystemlib.h"

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

#include "Madgine/serialize/filesystem/filemanager.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/hierarchy/statetransmissionflags.h"

METATABLE_BEGIN_BASE(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
#if ENABLE_PLUGINS
PROPERTY(ProjectRoot, projectRootString, setProjectRoot)
PROPERTY(Layout, layout, setLayout)
#endif
METATABLE_END(Engine::Tools::ProjectManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::ProjectManager, Engine::Tools::ToolBase)
#if ENABLE_PLUGINS
ENCAPSULATED_FIELD(ProjectRoot, projectRoot, setProjectRoot)
ENCAPSULATED_FIELD(Layout, layout, setLayout)
#endif
SERIALIZETABLE_END(Engine::Tools::ProjectManager)

UNIQUECOMPONENT(Engine::Tools::ProjectManager)

namespace Engine {
namespace Tools {

    ProjectManager::ProjectManager(ImRoot &root)
        : Tool<ProjectManager>(root, true)
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

    void ProjectManager::render()
    {
        /* ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoBackground);
        ImGui::End();*/
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

        static std::string layoutNameBuffer;
        if (ImGui::BeginPopupModal("NewLayout")) {

            ImGui::InputText("Name", &layoutNameBuffer);
            if (layoutNameBuffer.empty()) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Ok")) {
                setLayout(layoutNameBuffer);

                ImGui::CloseCurrentPopup();
            }
            if (layoutNameBuffer.empty()) {
                ImGui::EndDisabled();
            }
            ImGui::SameLine();
            if (ImGui::Button("Abort"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        bool openProjectDialog = false;
        bool openLayoutDialog = false;

        if (ImGui::BeginMenu("Project")) {

            if (ImGui::MenuItem("New Project...")) {
                throw 0;
            }

            if (ImGui::MenuItem("Open Project...")) {
                openProjectDialog = true;
            }

            ImGui::Separator();

            if (mProjectRoot.empty())
                ImGui::BeginDisabled();
            if (ImGui::MenuItem("New Layout...")) {
                openLayoutDialog = true;
            }
            if (ImGui::MenuItem("Save Layout")) {
                save();
            }

            ImGui::Separator();

            for (const std::string &layout : projectLayouts()) {
                if (ImGui::MenuItem(layout.c_str(), nullptr, mLayout == layout)) {
                    setLayout(layout);
                }
            }
            if (mProjectRoot.empty())
                ImGui::EndDisabled();
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
        if (openLayoutDialog) {
            layoutNameBuffer.clear();
            ImGui::OpenPopup("NewLayout");
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
            const std::vector<std::string> &layouts = projectLayouts();
            mLayout = layouts.empty() ? "" : layouts.front();

            load();

            if (!mProjectRoot.empty()) {
                Resources::ResourceManager::getSingleton().registerResourceLocation(mProjectRoot / "data", 80);
            }
        }
    }

    std::vector<std::string> ProjectManager::projectLayouts() const
    {
        if (mProjectRoot.empty())
            return {};
        std::vector<std::string> result;
        for (const Filesystem::Path &p : Filesystem::listFiles(mProjectRoot / "data")) {
            if (p.extension() == ".layout") {
                result.push_back(std::string { p.stem() });
            }
        }
        return result;
    }

    void ProjectManager::setLayout(const std::string &layout)
    {
        assert(!mProjectRoot.empty() || layout.empty());
        if (mLayout != layout) {
            mLayout = layout;
            load();
        }
    }
#endif

    const Filesystem::Path &ProjectManager::projectRoot() const
    {
        return mProjectRoot;
    }

    const std::string &ProjectManager::projectRootString() const
    {
        return projectRoot().str();
    }

    const std::string &ProjectManager::layout() const
    {
        return mLayout;
    }

    void ProjectManager::save()
    {
        Filesystem::Path folder = mProjectRoot / "data";
        Filesystem::createDirectory(folder);
        Filesystem::Path filePath = folder / (mLayout + ".layout");

        Filesystem::FileManager file("Layout");
        Serialize::FormattedSerializeStream out = file.openWrite(filePath, std::make_unique<Serialize::XMLFormatter>());

        if (out) {
            Serialize::write(out, *mWindow, "Layout");
        } else {
            LOG_ERROR("Failed to open \"" << filePath << "\" for write!");
        }
    }

    void ProjectManager::load()
    {
        if (!mLayout.empty()) {

            Filesystem::Path filePath = mProjectRoot / "data" / (mLayout + ".layout");

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
