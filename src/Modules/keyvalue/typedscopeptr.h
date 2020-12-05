#pragma once

#include "metatable.h"

namespace Engine {

DERIVE_FUNCTION(customScopePtr);

struct MODULES_EXPORT TypedScopePtr {
private:
    template <typename T>
    TypedScopePtr(T *t, std::true_type)
        : TypedScopePtr(t ? t->customScopePtr() : TypedScopePtr {nullptr, table<decayed_t<T>>})
    {
    }

    template <typename T>
    TypedScopePtr(T *t, std::false_type)
        : mScope(t)
        , mType(table<decayed_t<T>>)
    {
    }

public:
    constexpr TypedScopePtr() = default;

    template <typename T, typename = std::enable_if_t<!std::is_same_v<ScopeBase, T> && (std::is_base_of_v<ScopeBase, T> || has_function_customScopePtr_v<T>)>>
    TypedScopePtr(T *t)
        : TypedScopePtr(t, has_function_customScopePtr<T> {})
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

    bool operator==(const TypedScopePtr &other) const
    {
        return mScope == other.mScope && mType == other.mType;
    }

    bool operator<(const TypedScopePtr &other) const
    {
        if (mScope < other.mScope)
            return true;
        if (mScope > other.mScope)
            return false;
        return mType < other.mType;
    }

    template <typename T>
    T *safe_cast() const
    {
        if (!mType->isDerivedFrom<std::remove_const_t<T>>())
            std::terminate();
        return static_cast<T *>(mScope);
    }

    operator bool() const
    {
        return mScope != nullptr;
    }

    ScopeIterator find(const std::string &key) const;
    ScopeField operator[](const std::string &key) const;
    //bool isEditable(const std::string &key) const;
    ScopeIterator begin() const;
    ScopeIterator end() const;

    std::string name() const;

    ScopeBase *mScope = nullptr;
    const MetaTable * mType = nullptr;
};

}