#include "../../maditorviewlib.h"

#include "inspectorlayoutdata.h"

#include <experimental/filesystem>
#include "imgui.h"

#include "../../../model/application/inspector/scopewrapper.h"

#include "../../../model/application/inspector/inspector.h"
#include "imgui_internal.h"

#include <array>

namespace Maditor {
	namespace View {

		std::map<QString, void (InspectorLayout::*)(QDomElement, Model::ScopeWrapperItem*, std::set<std::string>&)> InspectorLayout::sElements = {
			{"Member", &InspectorLayout::drawSingleElement},
			{"MemberList", &InspectorLayout::drawElementList},
			{"Inherit", &InspectorLayout::inheritLayout},
		{"String", &InspectorLayout::drawConstantString},
		{"Association", nullptr}
		};

		InspectorLayout::InspectorLayout(InspectorLayoutData *data, const QString& path) :
		mPath(path),
		mData(data)
		{			
			QFile file(path);
			if (!file.open(QIODevice::ReadOnly))
				throw 0;
			if (!mDocument.setContent(&file))
				throw 0;
			file.close();
		}

		void InspectorLayout::draw(Model::ScopeWrapperItem* scope, std::set<std::string>& drawn)
		{
			for (QDomElement el = mDocument.documentElement().firstChildElement(); !el.isNull(); el = el.nextSiblingElement())
			{
				auto it = sElements.find(el.tagName());
				if (it != sElements.end())
				{
					if (it->second)
						(this->*it->second)(el, scope, drawn);
				}else
				{
					ImGui::Text(("Unsupported Tag-Type: "s + el.tagName().toLatin1().data()).c_str());
				}

				
			}
		}

		QStringList InspectorLayout::associations()
		{
			QStringList result;
			for (QDomElement el = mDocument.documentElement().firstChildElement("Association"); !el.isNull(); el = el.nextSiblingElement("Association"))
			{
				result << el.text();
			}
			return result;
		}

		void InspectorLayoutData::drawRemainingMembers(Model::ScopeWrapperItem* scope, std::set<std::string>& drawn)
		{
			for (std::pair<const std::string, Model::ValueItem> &v : scope->values())
			{
				if (drawn.find(v.first) == drawn.end()) {					
					drawValue(QDomElement{}, &v.second);
					drawn.insert(v.first);
				}
			}
		}

		void InspectorLayout::drawSingleElement(QDomElement element, Model::ScopeWrapperItem* scope, std::set<std::string>& drawn)
		{
			std::string name = element.attribute("name").toLatin1().data();
			auto it = scope->values().find(name);
			if (it != scope->values().end())
			{
				drawn.insert(name);
				if (element.attribute("hide", "<not-found>") == "<not-found>")
					mData->drawValue(element, &it->second);
			}else
			{
				if (element.attribute("optional", "<not-found>") == "<not-found>")
				{
					ImGui::Text(("Required field not found: "s + name).c_str());
				}
			}
		}

		void InspectorLayout::drawElementList(QDomElement element, Model::ScopeWrapperItem* scope,
			std::set<std::string>& drawn)
		{
			std::string name = element.attribute("name").toLatin1().data();
			bool draw = name.empty() || ImGui::TreeNode(name.c_str());
			for (std::pair<const std::string, Model::ValueItem> &v : scope->values())
			{
				if (drawn.find(v.first) == drawn.end()) {
					bool skip = false;
					for (QDomElement condition = element.firstChildElement("Condition"); !condition.isNull(); condition = condition.nextSiblingElement("Condition"))
					{
						QString type = condition.attribute("type");
						if (!type.isEmpty() && type != v.second.type())
						{
							skip = true;
							break;
						}
					}
					if (skip)
						continue;

					if (draw) {
						

						QDomElement rule;
						QDomNodeList children = element.elementsByTagName("Rule");
						for (int i = 0; i < children.count(); ++i)
						{
							QDomElement el = children.at(i).toElement();
							std::string name = el.attribute("name").toLatin1().data();
							if (name.empty() || name == v.first)
							{
								rule = el;
							}
						}
						mData->drawValue(rule, &v.second);
					}
					drawn.insert(v.first);
				}
			}
			if (draw && !name.empty())
				ImGui::TreePop();
			

		}

		void InspectorLayout::inheritLayout(QDomElement element, Model::ScopeWrapperItem* scope, std::set<std::string>& drawn)
		{
			InspectorLayout *layout = mData->getLayout(element.attribute("name"));
			if (layout)
			{
				layout->draw(scope, drawn);
			}
			else
			{
				ImGui::Text((QString("Layout not found: ") + element.attribute("name")).toLatin1().data());
			}
		}

		void InspectorLayout::drawConstantString(QDomElement element, Model::ScopeWrapperItem* scope,
			std::set<std::string>& drawn)
		{
			ImGui::Text(element.text().toLatin1().data());
			if (element.attribute("nobreak", "<not-found>") != "<not-found>")
				ImGui::SameLine();
		}

		void InspectorLayoutData::drawValue(QDomElement element, Model::ValueItem *item)
		{
			bool showName = element.attribute("noname", "<not-found>") == "<not-found>";
			std::string id = (showName ? std::string() : "##"s) + item->name();
			std::string name = showName ? item->name() : std::string();
			Engine::ValueType value = item->value();
			bool editable = (item->flags() & Engine::IsEditable) == Engine::IsEditable;

			if (!editable && value.type() != Engine::ValueType::Type::InvScopePtrValue)
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

			switch(value.type())
			{
			case Engine::ValueType::Type::InvScopePtrValue:
				if (element.attribute("recursive", "") != "<not-found>") {
					Engine::InvScopePtr ptr = value.as<Engine::InvScopePtr>();
					size_t &refcount = mScopes[ptr];
					bool wasHidden = false;
					if (refcount > 0) --refcount;
					else wasHidden = true;
					if (ImGui::TreeNodeEx(id.c_str()))
					{
						Model::ScopeWrapperItem *item = mInspector->getScope(ptr);
						QString layout = element.attribute("layout");
						if (!layout.isEmpty())
							item->setType(layout);
						drawScope(item);
						++refcount;
						ImGui::TreePop();
					}
					if (refcount == 0 && !wasHidden)
						mInspector->unregisterScope(ptr);
				}else
				{					
					if((showName && ImGui::Button(name.c_str())) || (!showName && ImGui::ArrowButton(item->name().c_str(), ImGuiDir_Right)))
					{
						
					}
				}
				
				break;
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
				std::array<float, 4> tmp = value.as<std::array<float,4>>();

				if (ImGui::DragFloat4(id.c_str(), tmp.data(), 0.15f))
					value = tmp;

				break;
			}
			default:
				ImGui::Text(name.c_str()); ImGui::SameLine(); 
				ImGui::Text(("Unsupported ValueType: "s + value.getTypeString()).c_str());
			}
			if (element.attribute("nobreak", "<not-found>") != "<not-found>")
				ImGui::SameLine();

			if (!editable && value.type() != Engine::ValueType::Type::InvScopePtrValue)
				ImGui::PopItemFlag();

			if (value != item->value())
				item->requestSetValue(value);
		}

		Model::ScopeWrapperItem* InspectorLayoutData::registerIndex(Engine::InvScopePtr ptr)
		{
			++mScopes[ptr];
			return mInspector->registerScope(ptr);
		}

		Model::ScopeWrapperItem *InspectorLayoutData::updateIndex(Engine::InvScopePtr from, Engine::InvScopePtr to)
		{
			unregisterIndex(from);
			return registerIndex(to);
		}

		void InspectorLayoutData::unregisterIndex(Engine::InvScopePtr ptr)
		{
			assert(mScopes.at(ptr) > 0);
			if (--mScopes.at(ptr) == 0)
			{
				mInspector->unregisterScope(ptr);
			}
		}


		InspectorLayoutData::InspectorLayoutData() :
		mInspector(nullptr)
		{			
			for (const QFileInfo &info : QDir("../Maditor/layouts/").entryInfoList({"*.xml"}))
			{
				InspectorLayout &layout = mLayouts.try_emplace(info.baseName(), this, info.absoluteFilePath()).first->second;
				for (const QString &assoc : layout.associations())
				{
					mAssociations[assoc] = &layout;
				}
			}
		}

		void InspectorLayoutData::drawScope(Model::ScopeWrapperItem* scope) {
			std::set<std::string> drawn;
			QString type = scope->type();
			InspectorLayout *layout = nullptr;
			if (type.isEmpty())
			{				
				auto it = mAssociations.find(scope->key());
				if (it != mAssociations.end())
				{
					layout = it->second;
				}
			}else
			{
				layout = getLayout(type);
				if (!layout)			
				{
					ImGui::Text((QString("Layout not found: ") + type).toLatin1().data());
				}
			}
			if (layout)
			{
				layout->draw(scope, drawn);
			}
			

			drawRemainingMembers(scope, drawn);
		}

		InspectorLayout* InspectorLayoutData::getLayout(const QString& name)
		{
			
			auto it = mLayouts.find(name);
			if (it != mLayouts.end())
			{
				return &it->second;
			}
			return nullptr;
		}

		void InspectorLayoutData::setModel(Model::Inspector* inspector)
		{
			mInspector = inspector;
		}
	}
}
