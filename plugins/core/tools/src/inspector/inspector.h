#pragma once
#include "../toolscollector.h"

#include "Modules/keyvalue/typedscopeptr.h"

//#include "inspectorlayout.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT Inspector : Tool<Inspector> {
        Inspector(ImRoot &root);
        Inspector(const Inspector &) = delete;
        ~Inspector();

        virtual void render() override;

        void drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn);
        void drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, const ScopeIterator &it);
        //void drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, std::string id, std::string key, ValueType value, bool editable);

        void draw(TypedScopePtr scope, std::set<std::string> drawn = {}, const char *layoutName = nullptr);

        InspectorLayout *getLayout(const std::string &name);

        std::string_view key() const override;

        void addObjectSuggestion(const MetaTable *type, std::function<std::vector<std::pair<std::string, TypedScopePtr>>()> getter);
        template <typename T>
        void addObjectSuggestion(std::function<std::vector<std::pair<std::string, TypedScopePtr>>()> getter)
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
        void draw(InspectorLayout *layout, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

        void drawSingleElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawElementList(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void inheritLayout(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawConstantString(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawSingleLine(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

		bool drawValueImpl(tinyxml2::XMLElement *element, TypedScopePtr parent, const std::string &id, ValueType &value, bool editable);

    private:
        std::map<std::string, InspectorLayout *> mAssociations;
        std::map<std::string, InspectorLayout> mLayouts;

        std::map<const MetaTable *, std::function<std::vector<std::pair<std::string, TypedScopePtr>>()>> mObjectSuggestionsByType;
        std::map<const MetaTable *, std::function<void(TypedScopePtr)>> mPreviews;

        static std::map<std::string, void (Inspector::*)(tinyxml2::XMLElement *, TypedScopePtr, std::set<std::string> &)> sElements;
    };
}
}
