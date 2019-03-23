#ifndef STATIC_BUILD

#include "../toolslib.h"



#include "pluginmanager.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../renderer/imguiaddons.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/reflection/classname.h"

namespace Engine
{
	namespace Tools
	{

		PluginManager::PluginManager(ImGuiRoot& root) :
		Tool<PluginManager>(root),
			mManager(Plugins::PluginManager::getSingleton())
		{
			
		}

		void PluginManager::render()
		{
			if (ImGui::Begin("Plugin Manager"))
			{				
				static char nameBuffer[256];
				static char pathBuffer[256];

				if (ImGui::BeginPopupModal("fileSelect")) {
					
					ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));
					ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer));
					if (strlen(nameBuffer) == 0 || strlen(pathBuffer) == 0) 
					{
						ImGui::PushDisabled();
					}
					if (ImGui::Button("Ok"))
					{
						mManager.setCurrentSelection(nameBuffer, pathBuffer);
						ImGui::CloseCurrentPopup();
					}
					if (strlen(nameBuffer) == 0 || strlen(pathBuffer) == 0)
					{
						ImGui::PopDisabled();
					}
					ImGui::SameLine();
					if (ImGui::Button("Abort"))
						ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				bool openDialog = false;

				if (ImGui::BeginCombo("Plugin Selection file", mManager.currentSelectionName().c_str())) {
					for (const std::pair<const std::string, std::string> &p : mManager.selectionFiles())
					{
						bool is_selected = (mManager.currentSelectionName() == p.first); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(p.first.c_str(), is_selected))
							mManager.setCurrentSelection(p.first, p.second);
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
					ImGui::Separator();


					if (ImGui::Selectable("New ...")) {
						openDialog = true;
					}
					ImGui::EndCombo();
				}

				if (openDialog) {
					nameBuffer[0] = '\0';
					pathBuffer[0] = '\0';
					ImGui::OpenPopup("fileSelect");
				}

				if (mManager.currentSelectionName().empty()) {
					ImGui::PushDisabled();
				}
				if (ImGui::Button("Export")) {
					exportStaticComponentHeader(mManager.currentSelectionPath() / ("components_"s + mManager.currentSelectionName() + ".cpp"), {&typeInfo<PluginManager>()});
				}
				if (mManager.currentSelectionName().empty()) {
					ImGui::PopDisabled();
				}

				ImVec2 v = ImGui::GetContentRegionAvail();
				v.x *= 0.5f;

				ImGui::BeginChild("Child1", v, false, ImGuiWindowFlags_HorizontalScrollbar);

				for (std::pair<const std::pair<std::string, std::string>, Plugins::PluginSection> &sec : mManager) {
					if (ImGui::TreeNode(sec.first.first.c_str()))
					{
						if (ImGui::TreeNode(sec.first.second.c_str())) {
							for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
								if (sec.first.first == "Madgine" && ((sec.first.second == "Utility" && p.first == "Tools") || sec.first.second == "Core")) {
									ImGui::PushDisabled();
								}
								bool loaded = p.second.isLoaded();
								bool clicked = false;
								if (sec.second.isExclusive()) {
									clicked = ImGui::RadioButton(p.first.c_str(), loaded);
									if (clicked)
										loaded = true;
								}
								else
									clicked = ImGui::Checkbox(p.first.c_str(), &loaded);
								if (clicked) {
									if (loaded)
										sec.second.loadPlugin(p.first);
									else
										sec.second.unloadPlugin(p.first);
								}
								if (sec.first.first == "Madgine" && ((sec.first.second == "Utility" && p.first == "Tools") || sec.first.second == "Core")) {
									ImGui::PopDisabled();
								}
							}
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
				ImGui::EndChild();
				ImGui::SameLine();

				ImGui::BeginChild("Child2", v, false, ImGuiWindowFlags_HorizontalScrollbar);

				for (const std::pair<const std::pair<std::string, std::string>, Plugins::PluginSection> &sec : mManager) {
					for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
						if (p.second.isLoaded()) {
							const Plugins::BinaryInfo *binInfo = static_cast<const Plugins::BinaryInfo*>(p.second.getSymbol("binaryInfo"));
							
							if (ImGui::TreeNode(p.first.c_str())) {
								for (auto &[name, reg] : registryRegistry())
								{
									for (CollectorInfo *info : *reg) {
										if (info->mBinary == binInfo && ImGui::TreeNode(info->mBaseInfo->mTypeName)) {
											for (const TypeInfo *component : info->mElementInfos) {
												ImGui::Text("%s", component->mTypeName);
											}
											ImGui::TreePop();
										}
									}
								}
								ImGui::TreePop();
							}
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		const char* PluginManager::key()
		{
			return "Plugin Manager";
		}
	}
}



#endif