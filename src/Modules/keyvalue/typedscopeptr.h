#pragma once

#include "metatable.h"

namespace Engine {

    struct MODULES_EXPORT TypedScopePtr {
        ScopeBase *mScope;
        const MetaTable *mType;

        template <typename T, typename = std::enable_if_t<!std::is_same_v<ScopeBase, T>>>
        TypedScopePtr(T *t)
            : mScope(t)
            , mType(&table<T>())
        {
        }

		TypedScopePtr(const TypedScopePtr &other)
            : mScope(other.mScope)
            , mType(other.mType)
        {
        }

		TypedScopePtr(ScopeBase *scope, const MetaTable *type)
            : mScope(scope)
            , mType(type)
        {
        }

        bool
        operator==(const TypedScopePtr &other) const
        {
            return mScope == other.mScope && mType == other.mType;
        }

		ScopeIterator find(const std::string &key) const;
        std::optional<ValueType> get(const std::string &key) const;
        ScopeIterator begin() const;
        ScopeIterator end() const;
    };

}