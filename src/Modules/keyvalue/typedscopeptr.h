#pragma once

#include "metatable.h"

namespace Engine {

struct MODULES_EXPORT TypedScopePtr {
    ScopeBase *mScope;
    const MetaTable *mType;

    constexpr TypedScopePtr()
        : mScope(nullptr)
        , mType(nullptr)
    {
    }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<ScopeBase, T>>>
    TypedScopePtr(T *t)
        : mScope(t)
        , mType(&table<T>())
    {
    }

    constexpr TypedScopePtr(const TypedScopePtr &other)
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

    template <typename T>
    T *safe_cast() const
    {
        if (!mType->isInstance<std::remove_const_t<T>>())
            throw 0;
        return static_cast<T *>(mScope);
    }

    operator bool() const
    {
        return mScope != nullptr;
    }

    ScopeIterator find(const std::string &key) const;
    std::optional<ValueType> get(const std::string &key) const;
    void set(const std::string &key, const ValueType &value);
    bool isEditable(const std::string &key) const;
    ScopeIterator begin() const;
    ScopeIterator end() const;
};

}