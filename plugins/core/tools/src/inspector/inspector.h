#pragma once
#include "../toolscollector.h"
#include "../toolbase.h"

#include "Modules/keyvalue/typedscopeptr.h"
#include "Modules/keyvalue/boundapifunction.h"

//#include "inspectorlayout.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT Inspector : Tool<Inspector> {
        Inspector(ImRoot &root);
        Inspector(const Inspector &) = delete;
        ~Inspector();

        virtual void render() override;

        bool drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, const ScopeIterator &it);
        //void drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, std::string id, std::string key, ValueType value, bool editable);
        std::pair<bool, bool> drawValueImpl(tinyxml2::XMLElement *element, TypedScopePtr parent, const std::string &id, ValueType &value, bool editable);

        bool draw(TypedScopePtr scope, std::set<std::string> drawn = {}, const char *layoutName = nullptr);

        InspectorLayout *getLayout(const std::string &name);

        std::string_view key() const override;

        void addObjectSuggestion(const MetaTable *type, std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()> getter);
        template <typename T>
        void addObjectSuggestion(std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()> getter)
        {
            addObjectSuggestion(&table<T>(), std::move(getter));
        }

        void addPreviewDefinition(const MetaTable *type, std::function<void(TypedScopePtr)> preview);
        template <typename T>
        void addPreviewDefinition(std::function<void(T *)> preview)
        {
            addPreviewDefinition(table<T>, [=](TypedScopePtr p) { preview(p.safe_cast<T>()); });
        }

    private:
        bool draw(InspectorLayout *layout, TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

        bool drawSingleElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawElementList(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        bool inheritLayout(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawConstantString(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawSingleLine(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

		
    private:
        std::map<std::string, InspectorLayout *> mAssociations;
        std::map<std::string, InspectorLayout> mLayouts;

        std::map<const MetaTable *, std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()>> mObjectSuggestionsByType;
        std::map<const MetaTable *, std::function<void(TypedScopePtr)>> mPreviews;

        static std::map<std::string, bool (Inspector::*)(tinyxml2::XMLElement *, TypedScopePtr, std::set<std::string> &)> sElements;

        //FunctionTool
        std::string mCurrentPopupFunctionName;
        BoundApiFunction mCurrentPopupFunction;
        ArgumentList mCurrentPopupArguments;
    };
}
}

RegisterType(Engine::Tools::Inspector);