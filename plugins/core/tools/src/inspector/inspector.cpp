#include "../toolslib.h"
#include "Madgine/baselib.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "inspector.h"

#include "../renderer/imroot.h"
#include "imgui/imguiaddons.h"

#include "Modules/plugins/plugin.h"

#include "Modules/resources/resourcemanager.h"

#include "../tinyxml/tinyxml2.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalue/scopeiterator.h"

#include "Modules/keyvalue/keyvaluepair.h"

#include "functiontool.h"

#include "Madgine/app/application.h"

#include "inspectorlayout.h"

#include "Modules/keyvalueutil/keyvalueregistry.h"

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
            for (const MetaTable *type : loader->resourceTypes()) {
                addObjectSuggestion(type, [=]() {
                    return loader->resources();
                });
            }
        }
    }

    Inspector::~Inspector()
    {
    }

    void Inspector::render()
    {
        if (ImGui::Begin("Inspector", &mVisible)) {
            auto drawList = [this](const std::map<TypedScopePtr, const char *> &items) {
                for (const std::pair<const TypedScopePtr, const char *> &p : items) {
                    if (ImGui::TreeNode(p.second)) {
                        draw(p.first, {}, p.second);
                        ImGui::TreePop();
                    }
                }
            };

            drawList(KeyValueRegistry::globals());
            drawList(KeyValueRegistry::workgroupLocals());
        }
        ImGui::End();
    }

    void Inspector::drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn)
    {

        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.count(it->key()) == 0) {
                drawValue(nullptr, scope, it);
                drawn.insert(it->key());
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
        std::string id = (showName ? std::string() : "##"s) + it->key();
        bool editable = it->isEditable();

        ValueType value = *it;
        if (drawValueImpl(element, parent, id, value, editable))
            *it = value;
    }

    bool Inspector::drawValueImpl(tinyxml2::XMLElement *element, TypedScopePtr parent, const std::string &id, ValueType &value, bool editable)
    {
        bool cannotBeDisabled = value.index() == Engine::ValueTypeEnum::ScopeValue || value.index() == Engine::ValueTypeEnum::KeyValueVirtualRangeValue || value.index() == Engine::ValueTypeEnum::ApiFunctionValue;

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
                                                                     if (ImGui::BeginCombo((id + "##suggestions").c_str(), scope.name().c_str())) {
                                                                         for (std::pair<std::string_view, TypedScopePtr> p : it->second()) {
                                                                             if (ImGui::Selectable(p.first.data())) {
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
                                                                         draw(scope, {}, element ? element->Attribute("layout") : nullptr);
                                                                     }
                                                                     ImGui::TreePop();
                                                                 }
                                                                 return modified;
                                                             },
                                                     [&](const KeyValueVirtualRange &range) {
                                                         bool b = ImGui::TreeNodeEx(id.c_str());
                                                         ImGui::DraggableValueTypeSource(id, parent, value);
                                                         if (b) {
                                                             size_t i = 0;
                                                             for (auto [vKey, vValue] : range) {
                                                                 ValueType value = vValue;
                                                                 std::string key = std::to_string(i);
                                                                 if (!vKey.is<std::monostate>()) {
                                                                     key = vKey.toShortString() + "##" + key;
                                                                 } else if (value.is<TypedScopePtr>()) {
                                                                     key = "[" + std::to_string(i) + "] " + value.as<TypedScopePtr>().name() + "##" + key;
                                                                 }
                                                                 if (drawValueImpl(element, {}, key, value, /*editable && */ vValue.isEditable())) {
                                                                     vValue = value;
                                                                 }
                                                                 ++i;
                                                             }
                                                             ImGui::TreePop();
                                                         }
                                                         return false;
                                                     },
                                                     [&](BoundApiFunction &method) {
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

        return modified;
    }

    void Inspector::draw(TypedScopePtr scope, std::set<std::string> drawn, const char *layoutName)
    {
        if (!scope) {
            ImGui::Text("<NULL>");
            return;
        }

        const MetaTable *type = scope.mType;
        while (type) {
            const char *typeName = type->mTypeName;
            InspectorLayout *layout = nullptr;
            auto it = mAssociations.find(typeName);
            if (it != mAssociations.end()) {
                layout = it->second;
            } else {
                if (layoutName)
                    layout = getLayout(layoutName);
            }
            if (layout) {
                draw(layout, scope, drawn);
            }
            if (type->mBase)
                type = *type->mBase;
            else
                type = nullptr;
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
            ImGui::Text("Unsupported Tag-Type: %s", element->Name());
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
                ImGui::Text("Required field not found: %s", name);
            }
        }
    }

    void Inspector::drawElementList(tinyxml2::XMLElement *element, TypedScopePtr scope,
        std::set<std::string> &drawn)
    {
        const char *name = element->Attribute("name");
        bool draw = !name || ImGui::TreeNode(name);
        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.find(it->key()) == drawn.end()) {
                bool skip = false;
                for (tinyxml2::XMLElement *condition = element->FirstChildElement("Condition"); condition; condition = condition->NextSiblingElement("Condition")) {
                    const char *expectedType = condition->Attribute("type");
                    std::string type = it->value().is<TypedScopePtr>() ? it->value().as<TypedScopePtr>().mType->mTypeName : "";
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
                        if (!name || it->key() == std::string(name)) {
                            rule = child;
                        }
                    }
                    drawValue(rule, scope, it);
                }
                drawn.insert(it->key());
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
            ImGui::Text("Layout not found: %s", element->Attribute("name"));
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
        ImGui::PushMultiItemsWidths(count, 100.0f); //TODO
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

    std::string_view Inspector::key() const
    {
        return "Inspector";
    }

    void Inspector::addObjectSuggestion(const MetaTable *type, std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()> getter)
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
