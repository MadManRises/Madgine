#include "../toolslib.h"
#include "Madgine/baselib.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "inspector.h"

#include "../renderer/imroot.h"
#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/keyvalue/keyvalueiterate.h"

#include "Modules/plugins/plugin.h"

#include "Modules/plugins/binaryinfo.h"

#include "Modules/resources/resourcemanager.h"
#include "layoutloader.h"

#include "../tinyxml/tinyxml2.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalue/scopeiterator.h"

#include "functiontool.h"

#include "Madgine/app/application.h"

UNIQUECOMPONENT(Engine::Tools::Inspector);

namespace Engine {
namespace Tools {

    std::map<std::string, void (Inspector::*)(tinyxml2::XMLElement *, TypedScopePtr, std::set<std::string> &)> Inspector::sElements = {
        { "Member", &Inspector::drawSingleElement },
        { "MemberList", &Inspector::drawElementList },
        { "Inherit", &Inspector::inheritLayout },
        { "String", &Inspector::drawConstantString },
        { "Association", nullptr },
        { "SingleLine", &Inspector::drawSingleLine }
    };

    Inspector::Inspector(ImRoot &root)
        : Tool<Inspector>(root)
    {

        for (auto &[name, resource] : Resources::ResourceManager::getSingleton().get<LayoutLoader>()) {

            InspectorLayout &layout = mLayouts.try_emplace(name, resource.loadData()).first->second;
            for (const std::string &assoc : layout.associations()) {
                mAssociations[assoc] = &layout;
            }
        }

        for (Resources::ResourceLoaderBase *loader : uniquePtrToPtr(Resources::ResourceManager::getSingleton().mCollector)) {
            addObjectSuggestion(loader->resourceType(), [=]() {
                return loader->resources();
            });
        }
    }

    Inspector::~Inspector()
    {
    }

    void Inspector::render()
    {
        if (ImGui::Begin("Inspector", &mVisible)) {
            if (ImGui::TreeNode("Application")) {
                draw(&App::Application::getSingleton(), "Application");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Resources")) {
                draw(&Resources::ResourceManager::getSingleton(), "Resources");
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void Inspector::drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn)
    {

        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.find((*it).first) == drawn.end()) {
                drawValue(nullptr, scope, it);
                drawn.insert((*it).first);
            }
        }
    }

    static bool style(const char *styleAttr, tinyxml2::XMLElement *element)
    {
        const char *style = element->Attribute("style");
        if (!style)
            return false;

        const size_t len = strlen(styleAttr);

        while (const char *found = strstr(style, styleAttr)) {
            if ((found == style || found[-1] == ' ') && (found[len] == '\0' || found[len] == ' ')) {
                return true;
            }
            style = found + len;
        }
        return false;
    }

    void Inspector::drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, const ScopeIterator &it)
    {
        bool showName = !element || !style("noname", element);
        std::string id = (showName ? std::string() : "##"s) + (*it).first;
        bool editable = parent.isEditable((*it).first);

        drawValue(element, parent, id, (*it).first, (*it).second, editable);
    }

    void Inspector::drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, std::string id, std::string key, ValueType value, bool editable)
    {
        bool cannotBeDisabled = value.type() == Engine::ValueType::Type::ScopeValue || value.type() == Engine::ValueType::Type::KeyValueVirtualIteratorValue || value.type() == Engine::ValueType::Type::ApiMethodValue || value.type() == Engine::ValueType::Type::BoundApiMethodValue;

        if (!editable && !cannotBeDisabled)
            ImGui::PushDisabled();

        bool modified = ImGui::ValueType(&value, overloaded { [&](TypedScopePtr scope) {
                                                                 bool modified = false;

                                                                 auto it = mObjectSuggestionsByType.find(scope.mType);
                                                                 bool hasSuggestions = editable && it != mObjectSuggestionsByType.end();

                                                                 bool open;

                                                                 if (hasSuggestions) {
                                                                     ImGui::BeginTreeArrow(id.c_str());
                                                                     ImGui::SameLine();
                                                                     if (ImGui::BeginCombo("##suggestions", scope.name().c_str())) {
                                                                         for (std::pair<std::string, TypedScopePtr> p : it->second()) {
                                                                             if (ImGui::Selectable(p.first.c_str())) {
                                                                                 value = p.second;
                                                                                 modified = true;
                                                                             }
                                                                         }
                                                                         ImGui::EndCombo();
                                                                     }
                                                                     open = ImGui::EndTreeArrow();
                                                                 } else {
                                                                     open = ImGui::TreeNode(id.c_str());
                                                                 }                                                                 

                                                                 ImGui::DraggableValueTypeSource(id, parent, value, ImGuiDragDropFlags_SourceAllowNullID);
                                                                 if (editable && ImGui::BeginDragDropTarget()) {
                                                                     if (ImGui::AcceptDraggableValueType(scope, nullptr, [&](const TypedScopePtr &ptr) {
                                                                             return ptr.mType->isDerivedFrom(scope.mType);
                                                                         })) {
                                                                         value = scope;
                                                                         modified = true;
                                                                     }
                                                                     ImGui::EndDragDropTarget();
                                                                 }

                                                                 if (open) {
                                                                     if (scope) {
                                                                         draw(scope, element ? element->Attribute("layout") : nullptr);
                                                                     }
                                                                     ImGui::TreePop();
                                                                 }
                                                                 return modified;
                                                             },
                                                     [&](KeyValueVirtualIterator &it) {
                                                         bool b = ImGui::TreeNodeEx(id.c_str());
                                                         ImGui::DraggableValueTypeSource(id, parent, value);
                                                         if (b) {
                                                             for (; it != VirtualIteratorEnd {}; ++it) {
                                                                 drawValue(element, parent, (*it).first.toString(), (*it).first.toString(), (*it).second, false);
                                                             }
                                                             ImGui::TreePop();
                                                         }
                                                         return false;
                                                     },
                                                     [&](BoundApiMethod &method) {
                                                         std::string extended = "-> " + id;
                                                         if (ImGui::Button(extended.c_str())) {
                                                             getTool<FunctionTool>().setCurrentFunction(id, method);
                                                         }
                                                         ImGui::DraggableValueTypeSource(id, parent, value);
                                                         return false;
                                                     } },
            id.c_str());

        if (!editable && !cannotBeDisabled)
            ImGui::PopDisabled();

        if (modified)
            parent.set(key, value);
    }

    void Inspector::draw(TypedScopePtr scope, const char *layoutName)
    {
        std::set<std::string> drawn;
        const char *type = scope.mType->mTypeName;
        InspectorLayout *layout = nullptr;
        auto it = mAssociations.find(type);
        if (it != mAssociations.end()) {
            layout = it->second;
        } else {
            if (layoutName)
                layout = getLayout(layoutName);
        }
        if (layout) {
            draw(layout, scope, drawn);
        }
        drawRemainingMembers(scope, drawn);

        auto it2 = mPreviews.find(scope.mType);
        if (it2 != mPreviews.end()) {
            it2->second(scope);
        }
    }

    void Inspector::draw(InspectorLayout *layout, TypedScopePtr scope, std::set<std::string> &drawn)
    {
        for (tinyxml2::XMLElement *el = layout->rootElement()->FirstChildElement(); el; el = el->NextSiblingElement()) {
            drawElement(el, scope, drawn);
        }
    }

    void Inspector::drawElement(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        auto it = sElements.find(element->Name());
        if (it != sElements.end()) {
            if (it->second)
                (this->*it->second)(element, scope, drawn);
        } else {
            ImGui::Text("%s", ("Unsupported Tag-Type: "s + element->Name()).c_str());
        }
    }

    void Inspector::drawSingleElement(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        const char *name = element->Attribute("name");
        ScopeIterator value = scope.find(name);
        if (value != scope.end()) {
            drawn.insert(name);
            if (!style("hide", element)) {
                drawValue(element, scope, value);
            }
        } else {
            if (!style("optional", element)) {
                ImGui::Text("%s", ("Required field not found: "s + name).c_str());
            }
        }
    }

    void Inspector::drawElementList(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        const char *name = element->Attribute("name");
        bool draw = !name || ImGui::TreeNode(name);
        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.find((*it).first) == drawn.end()) {
                bool skip = false;
                for (tinyxml2::XMLElement *condition = element->FirstChildElement("Condition"); condition; condition = condition->NextSiblingElement("Condition")) {
                    const char *expectedType = condition->Attribute("type");
                    std::string type = (*it).second.is<TypedScopePtr>() ? (*it).second.as<TypedScopePtr>().mType->mTypeName : "";
                    if (expectedType && type != expectedType) {
                        skip = true;
                        break;
                    }
                }
                if (skip)
                    continue;

                if (draw) {
                    tinyxml2::XMLElement *rule = nullptr;
                    for (tinyxml2::XMLElement *child = element->FirstChildElement("Rule"); child; child = child->NextSiblingElement("Rule")) {
                        const char *name = child->Attribute("name");
                        if (!name || (*it).first == std::string(name)) {
                            rule = child;
                        }
                    }
                    drawValue(rule, scope, it);
                }
                drawn.insert((*it).first);
            }
        }
        if (draw && name)
            ImGui::TreePop();
    }

    void Inspector::inheritLayout(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        InspectorLayout *layout = getLayout(element->Attribute("name"));
        if (layout) {
            draw(layout, scope, drawn);
        } else {
            ImGui::Text("%s", ("Layout not found: "s + element->Attribute("name")).c_str());
        }
    }

    void Inspector::drawConstantString(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        ImGui::Text("%s", element->GetText());
    }

    void Inspector::drawSingleLine(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        int count = 0;
        for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement())
            ++count;
        ImGui::PushMultiItemsWidths(count, 1.0f); //TODO
        for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
            drawElement(child, scope, drawn);
            if (--count > 0)
                ImGui::SameLine();
            ImGui::PopItemWidth();
        }
    }

    InspectorLayout *Inspector::getLayout(const std::string &name)
    {
        auto it = mLayouts.find(name);
        if (it != mLayouts.end()) {
            return &it->second;
        }
        return nullptr;
    }

    const char *Inspector::key() const
    {
        return "Inspector";
    }

    void Inspector::addObjectSuggestion(const MetaTable *type, std::function<std::vector<std::pair<std::string, TypedScopePtr>>()> getter)
    {
        mObjectSuggestionsByType[type] = getter;
    }

    void Inspector::addPreviewDefinition(const MetaTable *type, std::function<void(TypedScopePtr)> preview)
    {
        mPreviews[type] = preview;
    }

}
}

METATABLE_BEGIN(Engine::Tools::Inspector)
METATABLE_END(Engine::Tools::Inspector)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Inspector, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Inspector)

RegisterType(Engine::Tools::Inspector);