#pragma once
#include "../toolbase.h"
#include "../toolscollector.h"

#include "Meta/keyvalue/boundapifunction.h"
#include "Meta/keyvalue/typedscopeptr.h"

#include "Meta/keyvalue/argumentlist.h"

//#include "inspectorlayout.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT Inspector : Tool<Inspector> {
        Inspector(ImRoot &root);
        Inspector(const Inspector &) = delete;
        ~Inspector();

        virtual void render() override;

        bool drawRemainingMembers(TypedScopePtr scope, std::set<std::string> &drawn);
        bool drawMember(TypedScopePtr parent, const ScopeIterator &it);
        std::pair<bool, bool> drawValue(std::string_view id, ValueType &value, bool editable, bool generic = false);
        std::pair<bool, bool> drawValue(std::string_view id, TypedScopePtr &scope, bool editable, ValueType *generic = nullptr);
        std::pair<bool, bool> drawValue(std::string_view id, OwnedScopePtr &scope, bool editable, ValueType *generic = nullptr);
        std::pair<bool, bool> drawValue(std::string_view id, ObjectPtr &object, bool editable, ValueType *generic = nullptr);
        bool drawValue(std::string_view id, KeyValueVirtualSequenceRange &range, bool editable);
        bool drawValue(std::string_view id, KeyValueVirtualAssociativeRange &range, bool editable);
        void drawValue(std::string_view id, BoundApiFunction &function, bool editable);

        bool drawMembers(TypedScopePtr scope, std::set<std::string> drawn = {});

        std::string_view key() const override;

        void addObjectSuggestion(const MetaTable *type, std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()> getter);
        template <typename T>
        void addObjectSuggestion(std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()> getter)
        {
            addObjectSuggestion(table<T>, std::move(getter));
        }
        bool hasObjectSuggestion(const MetaTable *type) const;

        void addPreviewDefinition(const MetaTable *type, std::function<void(TypedScopePtr)> preview);
        template <typename T>
        void addPreviewDefinition(std::function<void(T *)> preview)
        {
            addPreviewDefinition(table<T>, [=](TypedScopePtr p) { preview(p.safe_cast<T>()); });
        }

    private:
        std::map<const MetaTable *, std::function<std::vector<std::pair<std::string_view, TypedScopePtr>>()>> mObjectSuggestionsByType;
        std::map<const MetaTable *, std::function<void(TypedScopePtr)>> mPreviews;

        static std::map<std::string, bool (Inspector::*)(TypedScopePtr, std::set<std::string> &, tinyxml2::XMLElement *)> sElements;

        //FunctionTool
        std::string mCurrentPopupFunctionName;
        BoundApiFunction mCurrentPopupFunction;
        ArgumentList mCurrentPopupArguments;
    };
}
}

REGISTER_TYPE(Engine::Tools::Inspector)