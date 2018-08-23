#include "../toolslib.h"

#include "inspector.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../renderer/imguiroot.h"
#include "Madgine/app/application.h"

#include <experimental/filesystem>

#include "Madgine/generic/keyvalueiterate.h"

namespace Engine
{
	namespace Tools
	{

		std::map<std::string, void (InspectorLayout::*)(tinyxml2::XMLElement *, Scripting::ScopeBase *, std::set<std::string>&)> InspectorLayout::sElements = {
			{ "Member", &InspectorLayout::drawSingleElement },
		{ "MemberList", &InspectorLayout::drawElementList },
		{ "Inherit", &InspectorLayout::inheritLayout },
		{ "String", &InspectorLayout::drawConstantString },
		{ "Association", nullptr },
		{ "SingleLine", &InspectorLayout::drawSingleLine }
		};

		InspectorLayout::InspectorLayout(Inspector& inspector, const std::experimental::filesystem::path& path) :
		mInspector(inspector),
		mPath(path)
		{
			mDocument.LoadFile(path.generic_string().c_str());
		}

		void InspectorLayout::draw(Scripting::ScopeBase* scope, std::set<std::string>& drawn)
		{
			for (tinyxml2::XMLElement *el = mDocument.RootElement()->FirstChildElement(); el; el = el->NextSiblingElement())
			{
				drawElement(el, scope, drawn);
			}
		}

		void InspectorLayout::drawElement(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
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
				ImGui::Text((std::string("Unsupported Tag-Type: ") + element->Name()).c_str());
			}
		}

		std::vector<std::string> InspectorLayout::associations()
		{
			std::vector<std::string> result;
			for(tinyxml2::XMLElement *el = mDocument.RootElement()->FirstChildElement("Association"); el; el = el->NextSiblingElement("Association"))
			{
				result.push_back(el->GetText());
			}
			return result;
		}

		void InspectorLayout::drawSingleElement(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			const char *name = element->Attribute("name");
			std::unique_ptr<KeyValueIterator> value = scope->find(name);
			if (!value->ended())
			{
				drawn.insert(name);
				if (!element->Attribute("hide"))
				{
					mInspector.drawValue(element, scope, value);
				}
			}else
			{
				if (!element->Attribute("optional"))
				{
					ImGui::Text((std::string("Required field not found: ") + name).c_str());
				}
			}
		}

		void InspectorLayout::drawElementList(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
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
						mInspector.drawValue(rule, scope, it);
					}
					drawn.insert(it->key());
				}
			}
			if (draw && name)
				ImGui::TreePop();

		}

		void InspectorLayout::inheritLayout(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			InspectorLayout *layout = mInspector.getLayout(element->Attribute("name"));
			if (layout)
			{
				layout->draw(scope, drawn);
			}else
			{
				ImGui::Text((std::string("Layout not found: ") + element->Attribute("name")).c_str());
			}
		}

		void InspectorLayout::drawConstantString(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
			std::set<std::string>& drawn)
		{
			ImGui::Text(element->GetText());
		}

		void InspectorLayout::drawSingleLine(tinyxml2::XMLElement* element, Scripting::ScopeBase* scope,
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


		Inspector::Inspector(ImGuiRoot& root) :
		UniqueComponent(root)
		{
			Plugins::Plugin plugin("Base" CONFIG_SUFFIX SHARED_LIBRARY_SUFFIX);
			plugin.load();
			for (const std::experimental::filesystem::path &p : std::experimental::filesystem::directory_iterator(plugin.fullPath().parent_path() / "../Maditor/layouts"))
			{
				if (p.extension() != ".xml")
					continue;
				InspectorLayout &layout = mLayouts.try_emplace(p.stem().generic_string(), *this, absolute(p)).first->second;
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
					drawValue(nullptr, scope, it);
					drawn.insert(it->key());
				}
			}
		}

		void Inspector::drawValue(tinyxml2::XMLElement* element, Scripting::ScopeBase *parent, const std::unique_ptr<KeyValueIterator> &it)
		{
			bool showName = !element || !element->Attribute("noname");
			std::string id = (showName ? std::string() : std::string("##")) + it->key();
			std::string name = showName ? it->key() : std::string();
			ValueType value = it->value();
			bool editable = (it->flags() & Engine::IsEditable) == Engine::IsEditable;

			if (!editable && value.type() != Engine::ValueType::Type::ScopeValue)
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

			switch (value.type())
			{
			case Engine::ValueType::Type::ScopeValue:
			{
				Scripting::ScopeBase *scope = value.as<Scripting::ScopeBase*>();
				if (ImGui::TreeNodeEx(id.c_str()))
				{
					draw(scope, element ? element->Attribute("layout") : nullptr);
					ImGui::TreePop();
				}
				break;
			}
			case Engine::ValueType::Type::BoolValue:
			{
				bool tmp = value.as<bool>();
				if (ImGui::Checkbox(id.c_str(), &tmp))
					value = tmp;
				break;
			}
			case Engine::ValueType::Type::StringValue:
			{
				std::string tmp = value.as<std::string>();
				char buf[255];
				strcpy_s(buf, tmp.c_str());

				if (ImGui::InputText(id.c_str(), buf, sizeof(buf)))
					value = buf;
				break;
			}
			case Engine::ValueType::Type::IntValue:
			{
				int tmp = value.as<int>();
				if (ImGui::DragInt(id.c_str(), &tmp))
					value = tmp;
				break;
			}
			case Engine::ValueType::Type::UIntValue:
			{
				size_t tmp = value.as<size_t>();
				if (ImGui::DragScalar(id.c_str(), ImGuiDataType_U32, &tmp, 1.0f))
					value = tmp;
				break;
			}
			case Engine::ValueType::Type::Matrix3Value:
			{
				Matrix3 tmp = value.as<Matrix3>();
					
				ImGui::DragFloat3((id + "_1").c_str(), tmp[0], 0.15f);
				ImGui::DragFloat3((id + "_2").c_str(), tmp[1], 0.15f);
				ImGui::DragFloat3((id + "_3").c_str(), tmp[2], 0.15f);

				value = tmp;
				break;
			}
			case Engine::ValueType::Type::Vector2Value:
			{
				Engine::Vector2 tmp = value.as<Engine::Vector2>();

				if (ImGui::DragFloat2(id.c_str(), tmp.ptr(), 0.15f))
					value = tmp;

				break;
			}
			case Engine::ValueType::Type::Vector3Value:
			{
				Engine::Vector3 tmp = value.as<Engine::Vector3>();

				if (ImGui::DragFloat3(id.c_str(), tmp.ptr(), 0.15f))
					value = tmp;

				break;
			}
			case Engine::ValueType::Type::Vector4Value:
			{
				std::array<float, 4> tmp = value.as<std::array<float, 4>>();

				if (ImGui::DragFloat4(id.c_str(), tmp.data(), 0.15f))
					value = tmp;

				break;
			}
			case Engine::ValueType::Type::ApiMethodValue:
				break;
			default:
				ImGui::Text(name.c_str()); ImGui::SameLine();
				ImGui::Text((std::string("Unsupported ValueType: ") + value.getTypeString()).c_str());
			}
			
			if (!editable && value.type() != Engine::ValueType::Type::ScopeValue)
				ImGui::PopItemFlag();

			if (value != it->value())
				parent->set(it->key(), value);
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
				layout->draw(scope, drawn);
			}
			drawRemainingMembers(scope, drawn);
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
