#include "../toolslib.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "inspector.h"

#include "imgui/imguiaddons.h"

#include "Madgine/resources/resourcemanager.h"

#include "../tinyxml/tinyxml2.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/scopeiterator.h"

#include "Meta/keyvalue/keyvaluepair.h"

#include "functiontool.h"

#include "inspectorlayout.h"

#include "Madgine/core/keyvalueregistry.h"

UNIQUECOMPONENT(Engine::Tools::Inspector);


METATABLE_BEGIN_BASE(Engine::Tools::Inspector, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::Inspector)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Inspector, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Inspector)


namespace Engine {
namespace Tools {

    std::map<std::string, bool (Inspector::*)(TypedScopePtr, std::set<std::string> &, tinyxml2::XMLElement *)> Inspector::sElements = {
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

        for (std::unique_ptr<Resources::ResourceLoaderBase> &loader : Resources::ResourceManager::getSingleton().mCollector) {
            for (const MetaTable *type : loader->resourceTypes()) {
                addObjectSuggestion(type, [&]() {
                    return loader->resources();
                });
            }
        }

        addObjectSuggestion<FunctionTable>([]() {
            std::vector<std::pair<std::string_view, TypedScopePtr>> result;
            const FunctionTable *table = sFunctionList();
            while (table) {
                result.emplace_back(table->mName, const_cast<FunctionTable*>(table));
                table = table->mNext;
            }
            return result;
        });
    }

    Inspector::~Inspector()
    {
    }

    void Inspector::render()
    {
        if (ImGui::Begin("Inspector", &mVisible)) {
            auto drawList = [this](const std::map<std::string_view, TypedScopePtr> &items) {
                for (const std::pair<const std::string_view, TypedScopePtr> &p : items) {
                    if (ImGui::TreeNode(p.first.data())) {
                        drawMembers(p.second, {}, p.first.data());
                        ImGui::TreePop();
                    }
                }
            };

            drawList(KeyValueRegistry::globals());
            drawList(KeyValueRegistry::workgroupLocals());
        }
        ImGui::End();
    }

    bool Inspector::drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn)
    {
        bool changed = false;

        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.count(it->key()) == 0) {
                changed |= drawMember(scope, it);
                drawn.insert(it->key());
            }
        }

        return changed;
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

    bool Inspector::drawMember(TypedScopePtr parent, const ScopeIterator &it, tinyxml2::XMLElement *element)
    {
        ValueType value;
        if (streq(it->key(), "__proxy")) {
            it->value(value);
            return drawMembers(value.as<TypedScopePtr>(), {});
        }

        bool showName = !element || !style("noname", element);
        std::string id = (showName ? std::string() : "##"s) + it->key();
        bool editable = it->isEditable();
        bool generic = it->isGeneric();

        it->value(value);
        std::pair<bool, bool> modified = drawValue(id, value, editable, generic, element);

        if (modified.first || (modified.second && !value.isReference()))
            *it = value;
        return modified.first || modified.second;
    }

    std::pair<bool, bool> Inspector::drawValue(std::string_view id, ValueType &value, bool editable, bool generic, tinyxml2::XMLElement *element)
    {
        if (generic)
            ImGui::BeginValueType({ ExtendedValueTypeEnum::GenericType }, id.data());

        std::pair<bool, bool> modified = value.visit(overloaded { [&](TypedScopePtr& scope) {
                                                                     return drawValue(id, scope, editable, element);                                                        
                                                                 },
            [&](OwnedScopePtr scope) {
                return drawValue(id, scope, editable, element);
            },
            [&](KeyValueVirtualSequenceRange &range) {
                
                return std::make_pair(false, drawValue(id, range, editable, element));
            },
            [&](KeyValueVirtualAssociativeRange &range) {
                
                return std::make_pair(false, drawValue(id, range, editable, element));
            },
            [&](BoundApiFunction &function) {
                drawValue(id, function, editable, element);
                return std::make_pair(false, false);
            },
            [&](auto &other) {
                if (!editable)
                    ImGui::PushDisabled();
                std::pair<bool, bool> result = std::make_pair(ImGui::ValueTypeDrawer { id.data(), false }.draw(other), false);
                if (!editable)
                    ImGui::PopDisabled();
                return result;
            } });

        if (generic)
            modified.first |= ImGui::EndValueType(&value, { ExtendedValueTypeEnum::GenericType });

        return modified;
    }

    std::pair<bool, bool> Inspector::drawValue(std::string_view id, TypedScopePtr &scope, bool editable, tinyxml2::XMLElement *element)
    {
        bool modified = false;
        bool changed = false;

        auto it = mObjectSuggestionsByType.find(scope.mType);
        bool hasSuggestions = editable && it != mObjectSuggestionsByType.end();

        bool open;

        if (hasSuggestions) {
            ImGui::BeginTreeArrow(id.data());
            ImGui::SameLine();
            if (ImGui::BeginCombo((std::string { id } + "##suggestions").c_str(), scope.name().c_str())) {
                for (std::pair<std::string_view, TypedScopePtr> p : it->second()) {
                    if (ImGui::Selectable(p.first.data())) {
                        scope = p.second;
                        modified = true;
                    }
                }
                ImGui::EndCombo();
            }
            open = ImGui::EndTreeArrow();
        } else {
            open = ImGui::TreeNode(id.data());
        }

        ImGui::DraggableValueTypeSource(id, scope, ImGuiDragDropFlags_SourceAllowNullID);
        if (editable && ImGui::BeginDragDropTarget()) {
            if (ImGui::AcceptDraggableValueType(scope, nullptr, [&](const TypedScopePtr &ptr) {
                    return ptr.mType->isDerivedFrom(scope.mType);
                })) {
                modified = true;
            }
            /*OwnedScopePtr dummy;
                                                                         if (ImGui::AcceptDraggableValueType(dummy, nullptr, [&](const OwnedScopePtr &ptr) {
                                                                                 return ptr.type()->isDerivedFrom(scope.mType);
                                                                             })) {
                                                                             scope = dummy;
                                                                             modified = true;
                                                                         }*/
            ImGui::EndDragDropTarget();
        }

        if (open) {
            changed |= drawMembers(scope, {}, element ? element->Attribute("layout") : nullptr);
            ImGui::TreePop();
        }
        return std::make_pair(modified, changed);
    }

    std::pair<bool, bool> Inspector::drawValue(std::string_view id, OwnedScopePtr &scope, bool editable, tinyxml2::XMLElement *element)
    {        
        TypedScopePtr ptr = scope.get();
        return drawValue(id, ptr, editable, element);
    }

    bool Inspector::drawValue(std::string_view id, KeyValueVirtualSequenceRange &range, bool editable, tinyxml2::XMLElement *element)
    {
        bool changed = false;
        bool b = ImGui::TreeNodeEx(id.data());
        ImGui::DraggableValueTypeSource(id, range);
        if (b) {
            size_t i = 0;
            for (auto vValue : range) {
                ValueType value = vValue;
                std::string key = std::to_string(i);
                if (value.is<TypedScopePtr>()) {
                    key = "[" + std::to_string(i) + "] " + value.as<TypedScopePtr>().name() + "##" + key;
                } else if (value.is<OwnedScopePtr>()) {
                    key = "[" + std::to_string(i) + "] " + value.as<OwnedScopePtr>().name() + "##" + key;
                }
                std::pair<bool, bool> result = drawValue(key, value, /*editable && */ vValue.isEditable(), false, element);
                if (result.first)
                    vValue = value;
                changed |= result.second;
                ++i;
            }
            ImGui::TreePop();
        }
        return changed;
    }

    bool Inspector::drawValue(std::string_view id, KeyValueVirtualAssociativeRange &range, bool editable, tinyxml2::XMLElement *element)
    {
        bool changed = false;
        bool b = ImGui::TreeNodeEx(id.data());
        ImGui::DraggableValueTypeSource(id, range);
        if (b) {
            size_t i = 0;
            for (auto [vKey, vValue] : range) {
                ValueType value = vValue;
                std::string key = vKey.toShortString() + "##" + std::to_string(i);
                std::pair<bool, bool> result = drawValue(key, value, /*editable && */ vValue.isEditable(), false, element);
                if (result.first)
                    vValue = value;
                changed |= result.second;
                ++i;
            }
            ImGui::TreePop();
        }
        return changed;
    }

    void Inspector::drawValue(std::string_view id, BoundApiFunction &function, bool editable, tinyxml2::XMLElement *element)
    {
        std::string extended = "-> " + std::string { id };
        if (ImGui::Button(extended.c_str())) {
            getTool<FunctionTool>().setCurrentFunction(id, function);
        }
        ImGui::DraggableValueTypeSource(id, function);
    }

    bool Inspector::drawMembers(TypedScopePtr scope, std::set<std::string> drawn, const char *layoutName)
    {
        if (!scope) {
            ImGui::Text("<NULL>");
            return false;
        }

        bool changed = false;

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
                changed |= draw(layout, scope, drawn);
            }
            if (type->mBase)
                type = *type->mBase;
            else
                type = nullptr;
        }
        changed |= drawRemainingMembers(scope, drawn);

        auto it2 = mPreviews.find(scope.mType);
        if (it2 != mPreviews.end()) {
            it2->second(scope);
        }
        return changed;
    }

    bool Inspector::draw(InspectorLayout *layout, TypedScopePtr scope, std::set<std::string> &drawn)
    {
        bool changed = false;
        for (tinyxml2::XMLElement *el = layout->rootElement()->FirstChildElement(); el; el = el->NextSiblingElement()) {
            changed |= drawElement(scope, drawn, el);
        }
        return changed;
    }

    bool Inspector::drawElement(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        auto it = sElements.find(element->Name());
        if (it != sElements.end()) {
            if (it->second)
                return (this->*it->second)(scope, drawn, element);
        } else {
            ImGui::Text("Unsupported Tag-Type: %s", element->Name());
        }
        return false;
    }

    bool Inspector::drawSingleElement(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        const char *name = element->Attribute("name");
        ScopeIterator value = scope.find(name);
        if (value != scope.end()) {
            drawn.insert(name);
            if (!style("hide", element)) {
                return drawMember(scope, value, element);
            }
        } else {
            if (!style("optional", element)) {
                ImGui::Text("Required field not found: %s", name);
            }
        }
        return false;
    }

    bool Inspector::drawElementList(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        bool changed = false;

        const char *name = element->Attribute("name");
        bool draw = !name || ImGui::TreeNode(name);
        for (ScopeIterator it = scope.begin(); it != scope.end(); ++it) {
            if (drawn.find(it->key()) == drawn.end()) {
                bool skip = false;
                for (tinyxml2::XMLElement *condition = element->FirstChildElement("Condition"); condition; condition = condition->NextSiblingElement("Condition")) {
                    const char *expectedType = condition->Attribute("type");
                    ValueType typeV;
                    it->value(typeV);
                    std::string type = typeV.is<TypedScopePtr>() ? typeV.as<TypedScopePtr>().mType->mTypeName : "";
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
                    changed |= drawMember(scope, it, rule);
                }
                drawn.insert(it->key());
            }
        }
        if (draw && name)
            ImGui::TreePop();

        return changed;
    }

    bool Inspector::inheritLayout(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        InspectorLayout *layout = getLayout(element->Attribute("name"));
        if (layout) {
            return draw(layout, scope, drawn);
        } else {
            ImGui::Text("Layout not found: %s", element->Attribute("name"));
            return false;
        }
    }

    bool Inspector::drawConstantString(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        ImGui::Text("%s", element->GetText());
        return false;
    }

    bool Inspector::drawSingleLine(TypedScopePtr scope,
        std::set<std::string> &drawn, tinyxml2::XMLElement *element)
    {
        int count = 0;
        for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement())
            ++count;
        ImGui::PushMultiItemsWidths(count, 100.0f); //TODO
        for (tinyxml2::XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
            drawElement(scope, drawn, child);
            if (--count > 0)
                ImGui::SameLine();
            ImGui::PopItemWidth();
        }
        return false;
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

    bool Inspector::hasObjectSuggestion(const MetaTable *type) const
    {
        return mObjectSuggestionsByType.contains(type);
    }

    void Inspector::addPreviewDefinition(const MetaTable *type, std::function<void(TypedScopePtr)> preview)
    {
        mPreviews[type] = preview;
    }

}
}
