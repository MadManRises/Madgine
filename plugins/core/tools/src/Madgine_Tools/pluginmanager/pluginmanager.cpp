#include "../toolslib.h"

#if ENABLE_PLUGINS

#    include "pluginmanager.h"

#    include "imgui/imgui.h"
#    include "imgui/imgui_internal.h"

#    include "Modules/plugins/pluginmanager.h"

#    include "Meta/keyvalue/metatable_impl.h"
#    include "Meta/serialize/serializetable_impl.h"

#    include "Generic/guard.h"

#    include "Interfaces/filesystem/api.h"

#    include "Modules/plugins/plugin.h"
#    include "Modules/plugins/pluginsection.h"

#    include "Modules/uniquecomponent/uniquecomponentcollector.h"

#    include "Madgine/base/root.h"

#    include "imgui/imguiaddons.h"

UNIQUECOMPONENT(Engine::Tools::PluginManager);

namespace Engine {
namespace Tools {

    static Guard excludeFromExport {
        []() {
            Base::skipUniqueComponentOnExport(&typeInfo<PluginManager>);
        }
    };

    PluginManager::PluginManager(ImRoot &root)
        : Tool<PluginManager>(root)
        , mManager(Plugins::PluginManager::getSingleton())
    {
    }

    void PluginManager::render()
    {
        ImGui::SetNextWindowSize({ 550, 400 }, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Plugin Manager", &mVisible)) {

            if (ImGui::Button("Export (with Tools)")) {
                Base::exportStaticComponentHeader("components_default(tools).cpp", true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Export (without Tools)")) {
                Base::exportStaticComponentHeader("components_default.cpp", false);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 40, 40, 255));
            ImGui::Text("Changes are only applied on restart!");
            ImGui::PopStyleColor();

            for (auto &[sectionName, section] : mManager) {
                if (ImGui::CollapsingHeader(sectionName.c_str())) {
                    for (auto &[pluginName, plugin] : section) {
                        const std::string &project = plugin.project();

                        bool loaded = plugin.isLoaded();

                        bool clicked = false;
                        std::string displayName { pluginName + " (" + project + ")" };
                        ImGui::BeginTreeArrow(&plugin);
                        ImGui::SameLine();
                        if (plugin.isDependencyOf(PLUGIN_SELF)) {
                            ImGui::BeginDisabled();
                        }
                        if (section.isExclusive()) {
                            clicked = ImGui::RadioButton(displayName.c_str(), loaded);
                            if (clicked)
                                loaded = true;
                        } else
                            clicked = ImGui::Checkbox(displayName.c_str(), &loaded);
                        if (clicked) {
                            if (loaded) {
                                if (section.loadPlugin(pluginName))
                                    updateConfigFile();
                            } else {
                                if (!section.unloadPlugin(pluginName))
                                    updateConfigFile();
                            }
                        }
                        if (plugin.isDependencyOf(PLUGIN_SELF)) {
                            ImGui::EndDisabled();
                        }
                        if (ImGui::EndTreeArrow()) {
                            if (loaded) {
                                const Plugins::BinaryInfo *binInfo = plugin.info();

                                const char **dep = binInfo->mPluginDependencies;
                                if (*dep && ImGui::TreeNode("Dependencies")) {
                                    while (*dep) {
                                        ImGui::Text("%s", *dep);
                                        ++dep;
                                    }
                                    ImGui::TreePop();
                                }

                                if (ImGui::TreeNode("UniqueComponents")) {
                                    for (UniqueComponent::RegistryBase *reg : UniqueComponent::registryRegistry()) {
                                        for (UniqueComponent::CollectorInfoBase *info : *reg) {
                                            if (info->mBinary == binInfo && ImGui::TreeNode(info->mBaseInfo->mTypeName.data(), "%.*s", static_cast<int>(info->mBaseInfo->mTypeName.size()), info->mBaseInfo->mTypeName.data())) {
                                                for (const std::vector<const TypeInfo *> &components : info->mElementInfos) {
                                                    ImGui::Text(components.front()->mTypeName);
                                                }
                                                ImGui::TreePop();
                                            }
                                        }
                                    }
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }                    
                }
            }
        }
        ImGui::End();
    }

    Threading::Task<bool> PluginManager::init()
    {
        //ProjectManager &project = getTool<ProjectManager>();

        //project.mProjectChanged.connect(&PluginManager::updateConfigFile, this);
        //setCurrentConfig(project.projectRoot(), project.config());

        co_return co_await ToolBase::init();
    }

    std::string_view PluginManager::key() const
    {
        return "Plugin Manager";
    }

    /* void PluginManager::setCurrentConfig(const Filesystem::Path &path, const std::string &name)
    {
        if (!name.empty()) {
            Filesystem::Path p = path / (name + ".cfg");
            if (Filesystem::exists(p)) {
                mManager.loadFromFile(p, true);
            }
        }
        updateConfigFile();
    }
    */
    void PluginManager::updateConfigFile()
    {
        /* ProjectManager &project = getTool<ProjectManager>();
        if (!project.config().empty()) {
            Filesystem::Path p = project.projectRoot() / (project.config() + "(tools).cfg");
            mManager.saveToFile(p, true);
            Filesystem::Path p_notools = project.projectRoot() / (project.config() + ".cfg");
            mManager.saveToFile(p_notools, false);
        }*/
    }
}
}

METATABLE_BEGIN_BASE(Engine::Tools::PluginManager, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::PluginManager)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::PluginManager, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::PluginManager)

#endif