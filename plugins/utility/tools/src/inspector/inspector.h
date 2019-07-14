#pragma once
#include "../tool.h"

#include "inspectorlayout.h"

#include "Modules/keyvalue/typedscopeptr.h"

namespace Engine {
namespace Tools {

    class MADGINE_TOOLS_EXPORT Inspector : public Tool<Inspector> {
    public:
        Inspector(ImGuiRoot &root);
        Inspector(const Inspector &) = delete;
        ~Inspector();

        virtual void render() override;

        void drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn);
        void drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, const ScopeIterator &it);
        void drawValue(tinyxml2::XMLElement *element, TypedScopePtr parent, std::string id, ValueType value, bool editable);

        InspectorLayout *getLayout(const std::string &name);

        const char *key() override;

    private:
        void draw(TypedScopePtr scope, const char *layoutName);

        void draw(InspectorLayout *layout, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

        void drawSingleElement(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawElementList(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void inheritLayout(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawConstantString(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);
        void drawSingleLine(tinyxml2::XMLElement *element, TypedScopePtr scope, std::set<std::string> &drawn);

    private:
        std::map<std::string, InspectorLayout *> mAssociations;
        std::map<std::string, InspectorLayout> mLayouts;

        static std::map<std::string, void (Inspector::*)(tinyxml2::XMLElement *, TypedScopePtr, std::set<std::string> &)> sElements;
    };
}
}
