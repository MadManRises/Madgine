#include "../toolslib.h"

#include "inspector.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../renderer/imguiroot.h"
#include "../renderer/imguiaddons.h"
#include "Madgine/app/application.h"

#include <experimental/filesystem>

#include "Interfaces/scripting/types/scopebase.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/plugins/plugin.h"

#include "Interfaces/plugins/binaryinfo.h"

#include "layoutloader.h"
#include "Madgine/resources/resourcemanager.h"

#include "../tinyxml/tinyxml2.h"

namespace Engine
{
	namespace Tools
	{


		std::map<std::string, void (Inspector::*)(tinyxml2::XMLElement *, Scripting::ScopeBase *, std::set<std::string>&)> Inspector::sElements = {
			{ "Member", &Inspector::drawSingleElement },
			{ "MemberList", &Inspector::drawElementList },
			{ "Inherit", &Inspector::inheritLayout },
			{ "String", &Inspector::drawConstantString },
			{ "Association", nullptr },
			{ "SingleLine", &Inspector::drawSingleLine }
		};

		Inspector::Inspector(ImGuiRoot& root) :
			Tool<Inspector>(root)
		{			
			
			for (std::pair<const std::string, LayoutLoader::ResourceType> &resource : Resources::ResourceManager::getSingleton().get<LayoutLoader>())
			{
				
				InspectorLayout &layout = mLayouts.try_emplace(resource.first, resource.second.loadImpl()).first->second;
				for (const std::string &assoc : layout.associations())
				{
					mAssociations[assoc] = &layout;
				}
			}
		}

		void Inspector::render()
		{
			if (ImGui::Begin("Inspector"))
			{
				draw(&root().app(), "Application");
				
			}
			ImGui::End();
		}

		void Inspector::drawRemainingMembers(Scripting::ScopeBase* scope, std::set<std::string>& drawn)
		{
			
			for (std::unique_ptr<KeyValueIterator> it = scope->iterator(); !it->ended(); ++(*it))
			{
				if (drawn.find(it->key()) == drawn.end()) {
					drawValue(nullptr, scope, *it);
					drawn.insert(it->key());
				}
			}
		}

		void Inspector::drawValue(tinyxml2::XMLElement* element, Scripting::ScopeBase *parent, const KeyValueIterator &it)
		{
			bool showName = !element || !element->Attribute("noname");
			std::string id = (showName ? std::string() : "##"s) + it.key();
			std::string name = showName ? it.key() : std::string();
			ValueType value = it.value();
			bool editable = (it.flags() & Engine::IsEditable) == Engine::IsEditable;

			if (!editable && value.type() != Engine::ValueType::Type::ScopeValue)
				ImGui::PushDisabled();

			value.visit(
				overloaded{
					[&](Scripting::ScopeBase *scope) 
					{
						if (ImGui::TreeNodeEx(id.c_str()))
						{
							draw(scope, element ? element->Attribute("layout") : nullptr);
							ImGui::TreePop();
						}
					},
					[&](bool tmp) 
					{
						if (ImGui::Checkbox(id.c_str(), &tmp))
							value = tmp;
					},
					[&](std::string tmp)
					{
						char buf[255];
						strcpy_s(buf, tmp.c_str());

						if (ImGui::InputText(id.c_str(), buf, sizeof(buf)))
							value = buf;
					},
					[&](int tmp)
					{
						if (ImGui::DragInt(id.c_str(), &tmp))
							value = tmp;
					},
					[&](size_t tmp)
					{
						if (ImGui::DragScalar(id.c_str(), ImGuiDataType_U32, &tmp, 1.0f))
							value = tmp;
					},
					[&](float tmp)
					{
						if (ImGui::DragFloat(id.c_str(), &tmp, 0.15f))
							value = tmp;
					},
					[&](Matrix3 tmp)
					{
						ImGui::DragFloat3((id + "_1").c_str(), tmp[0], 0.15f);
						ImGui::DragFloat3((id + "_2").c_str(), tmp[1], 0.15f);
						ImGui::DragFloat3((id + "_3").c_str(), tmp[2], 0.15f);

						value = tmp;
					},
					[&](Vector2 tmp)
					{
						if (ImGui::DragFloat2(id.c_str(), tmp.ptr(), 0.15f))
							value = tmp;
					},
					[&](Vector3 tmp)
					{
						if (ImGui::DragFloat3(id.c_str(), tmp.ptr(), 0.15f))
							value = tmp;
					},
					[&](Vector4 tmp)
					{
						if (ImGui::DragFloat4(id.c_str(), tmp.ptr(), 0.15f))
							value = tmp;
					},
					[&](auto)
					{
						ImGui::Text(name.c_str()); ImGui::SameLine();
						ImGui::Text(("Unsupported ValueType: "s + value.getTypeString()).c_str());
					}
				}
			);

			
			if (!editable && value.type() != Engine::ValueType::Type::ScopeValue)
				ImGui::PopDisabled();

			if (value != it.value())
				parent->set(it.key(), value);
		}

		void Inspector::draw(Scripting::ScopeBase* scope, const char *layoutName)
		{
			std::set<std::string> drawn;
			std::string type = scope->getIdentifier();
			InspectorLayout *layout = nullptr;
			auto it = mAssociations.find(type);
			if (it != mAssociations.end())
			{
				layout = it->second;
			}
			else {
				if (layoutName)
					layout = getLayout(layoutName);
			}
			if (layout)
			{
				draw(layout, scope, drawn);
			}
			drawRemainingMembers(scope, drawn);
		}

		void Inspector::draw(InspectorLayout *layout, Scripting::ScopeBase* scope, std::set<std::string>& drawn)
		{
			for (tinyxml2::XMLElement *el = layout->rootElement()->FirstChildElement(); el; el = el->NextSiblingElement())
			{
				drawElement(el, scope, drawn);
			}
		}

		void Inspector::drawElement(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			auto it = sElements.find(element->Name());
			if (it != sElements.end())
			{
				if (it->second)
					(this->*it->second)(element, scope, drawn);
			}
			else
			{
				ImGui::Text(("Unsupported Tag-Type: "s + element->Name()).c_str());
			}
		}


		void Inspector::drawSingleElement(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			const char *name = element->Attribute("name");
			std::unique_ptr<KeyValueIterator> value = scope->find(name);
			if (!value->ended())
			{
				drawn.insert(name);
				if (!element->Attribute("hide"))
				{
					drawValue(element, scope, *value);
				}
			}
			else
			{
				if (!element->Attribute("optional"))
				{
					ImGui::Text(("Required field not found: "s + name).c_str());
				}
			}
		}

		void Inspector::drawElementList(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			const char *name = element->Attribute("name");
			bool draw = !name || ImGui::TreeNode(name);
			for (std::unique_ptr<KeyValueIterator> it = scope->iterator(); !it->ended(); ++(*it))
			{
				if (drawn.find(it->key()) == drawn.end())
				{
					bool skip = false;
					for (tinyxml2::XMLElement *condition = element->FirstChildElement("Condition"); condition; condition = condition->NextSiblingElement("Condition"))
					{
						const char *expectedType = condition->Attribute("type");
						std::string type = it->value().is<Scripting::ScopeBase*>() ? it->value().as<Scripting::ScopeBase*>()->getIdentifier() : "";
						if (expectedType && type != expectedType)
						{
							skip = true;
							break;
						}
					}
					if (skip)
						continue;

					if (draw)
					{
						tinyxml2::XMLElement *rule = nullptr;
						for (tinyxml2::XMLElement *child = element->FirstChildElement("Rule"); child; child = child->NextSiblingElement("Rule"))
						{
							const char *name = child->Attribute("name");
							if (!name || it->key() == name)
							{
								rule = child;
							}
						}
						drawValue(rule, scope, *it);
					}
					drawn.insert(it->key());
				}
			}
			if (draw && name)
				ImGui::TreePop();

		}

		void Inspector::inheritLayout(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			InspectorLayout *layout = getLayout(element->Attribute("name"));
			if (layout)
			{
				draw(layout, scope, drawn);
			}
			else
			{
				ImGui::Text(("Layout not found: "s + element->Attribute("name")).c_str());
			}
		}

		void Inspector::drawConstantString(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			ImGui::Text(element->GetText());
		}

		void Inspector::drawSingleLine(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			size_t count = 0;
			for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement())
				++count;
			ImGui::PushMultiItemsWidths(count);
			for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement())
			{
				drawElement(child, scope, drawn);
				if (--count > 0)
					ImGui::SameLine();
				ImGui::PopItemWidth();
			}
		}


		InspectorLayout* Inspector::getLayout(const std::string& name)
		{
			auto it = mLayouts.find(name);
			if (it != mLayouts.end())
			{
				return &it->second;
			}
			return nullptr;
		}

		const char* Inspector::key()
		{
			return "Inspector";
		}
	}
}
