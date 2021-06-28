#include "../toolslib.h"

#if ENABLE_PLUGINS

#    include "pluginmanager.h"

#    include "imgui/imgui.h"
#    include "imgui/imgui_internal.h"

#    include "imgui/imguiaddons.h"

#    include "Modules/plugins/pluginmanager.h"

#    include "Meta/keyvalue/metatable_impl.h"
#    include "Meta/serialize/serializetable_impl.h"

#    include "Modules/threading/taskguard.h"

#    include "Interfaces/filesystem/api.h"

#    include "../project/projectmanager.h"

#    include "Modules/plugins/plugin.h"
#    include "Modules/plugins/pluginsection.h"

#    include "ini/inifile.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Tools::PluginManager);

namespace Engine {
namespace Tools {

    static Threading::TaskGuard excludeFromExport {
        []() {
            skipUniqueComponentOnExport(&typeInfo<PluginManager>);
        },
        {}
    };

    PluginManager::PluginManager(ImRoot &root)
        : Tool<PluginManager>(root)
        , mManager(Plugins::PluginManager::getSingleton())
    {
    }

    void PluginManager::render()
    {
        if (ImGui::Begin("Plugin Manager", &mVisible)) {

            ProjectManager &project = getTool<ProjectManager>();
            const Filesystem::Path &projectRoot = project.projectRoot();
            const std::string &config = project.config();

            if (projectRoot.empty()) {
                ImGui::Text("Please open a Project to modify the plugin selections.");
            } else {

                if (config.empty()) {
                    ImGui::PushDisabled();
                }
                if (ImGui::Button("Export (with Tools)")) {
                    exportStaticComponentHeader(projectRoot / ("components_"s + config + "(tools).cpp"), true);
                }
                ImGui::SameLine();
                if (ImGui::Button("Export (without Tools)")) {
                    exportStaticComponentHeader(projectRoot / ("components_"s + config + ".cpp"), false);
                }
                if (config.empty()) {
                    ImGui::PopDisabled();
                }

                ImVec2 v = ImGui::GetContentRegionAvail();
                v.x *= 0.5f;

                ImGui::BeginChild("Child1", v, false, ImGuiWindowFlags_HorizontalScrollbar);

                for (auto &[sectionName, section] : mManager) {
                    if (ImGui::TreeNode(sectionName.c_str())) {
                        for (auto &[pluginName, plugin] : section) {
                            const std::string &project = plugin.project();

                            if (plugin.isDependencyOf(PLUGIN_SELF)) {
                                ImGui::PushDisabled();
                            }
                            bool loaded;
                            SharedFuture<bool> state = plugin.state();
                            bool available = state.is_ready();
                            if (available)
                                loaded = state.get();
                            else
                                ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
                            bool clicked = false;
                            std::string displayName { pluginName + " (" + project + ")" };
                            if (section.isExclusive()) {
                                clicked = ImGui::RadioButton(displayName.c_str(), loaded);
                                if (clicked)
                                    loaded = true;
                            } else
                                clicked = ImGui::Checkbox(displayName.c_str(), &loaded);
                            if (available) {
                                if (clicked) {
                                    if (loaded)
                                        section.loadPlugin(pluginName, [this]() { updateConfigFile(); });
                                    else
                                        section.unloadPlugin(pluginName, [this]() { updateConfigFile(); });
                                }
                            } else {
                                ImGui::PopItemFlag();
                            }
                            if (plugin.isDependencyOf(PLUGIN_SELF)) {
                                ImGui::PopDisabled();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::EndChild();
                ImGui::SameLine();

                ImGui::BeginChild("Child2", v, false, ImGuiWindowFlags_HorizontalScrollbar);

                for (Plugins::PluginSection &section : kvValues(mManager)) {
                    for (auto &[pluginName, plugin] : section) {
                        if (plugin.isLoaded()) {
                            const Plugins::BinaryInfo *binInfo = static_cast<const Plugins::BinaryInfo *>(plugin.getSymbol("binaryInfo"));

                            if (ImGui::TreeNode(pluginName.c_str())) {
                                const char **dep = binInfo->mPluginDependencies;
                                if (*dep && ImGui::TreeNode("Dependencies")) {
                                    while (*dep) {
                                        ImGui::Text("%s", *dep);
                                        ++dep;
                                    }
                                    ImGui::TreePop();
                                }

                                if (ImGui::TreeNode("UniqueComponents")) {
                                    for (UniqueComponentRegistryBase *reg : registryRegistry()) {
                                        for (CollectorInfoBase *info : *reg) {
                                            if (info->mBinary == binInfo && ImGui::TreeNode(info->mBaseInfo->mTypeName.data(), "%.*s", static_cast<int>(info->mBaseInfo->mTypeName.size()), info->mBaseInfo->mTypeName.data())) {
                                                for (const std::vector<const TypeInfo *> &components : info->mElementInfos) {
                                                    ImGui::Text("%.*s", static_cast<int>(components.front()->mTypeName.size()), components.front()->mTypeName.data());
                                                }
                                                ImGui::TreePop();
                                            }
                                        }
                                    }
                                    ImGui::TreePop();
                                }
                                ImGui::TreePop();
                            }
                        }
                    }
                }
                ImGui::EndChild();
            }
        }
        ImGui::End();
    }

    bool PluginManager::init()
    {
        ProjectManager &project = getTool<ProjectManager>();

        project.mProjectChanged.connect(&PluginManager::updateConfigFile, this);
        setCurrentConfig(project.projectRoot(), project.config());

        return ToolBase::init();
    }

    std::string_view PluginManager::key() const
    {
        return "Plugin Manager";
    }

    void PluginManager::setCurrentConfig(const Filesystem::Path &path, const std::string &name)
    {
        if (!name.empty()) {
            Filesystem::Path p = path / (name + ".cfg");
            if (Filesystem::exists(p)) {
                mManager.loadFromFile(p);
            }
        }
        updateConfigFile();
    }

    void PluginManager::updateConfigFile()
    {
        ProjectManager &project = getTool<ProjectManager>();
        if (!project.config().empty()) {
            Filesystem::Path p = project.projectRoot() / (project.config() + "(tools).cfg");
            mManager.saveToFile(p, true);            
            Filesystem::Path p_notools = project.projectRoot() / (project.config() + ".cfg");
            mManager.saveToFile(p_notools, false);
        }
    }
}
}

METATABLE_BEGIN_BASE(Engine::Tools::PluginManager, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::PluginManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::PluginManager, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::PluginManager)

#endif