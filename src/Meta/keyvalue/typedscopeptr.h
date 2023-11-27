#pragma once

#include "metatable.h"

namespace Engine {

DERIVE_FUNCTION(customScopePtr)

struct META_EXPORT TypedScopePtr {

    constexpr TypedScopePtr() = default;
    
    template <typename T>
    requires has_function_customScopePtr_v<T>
    TypedScopePtr(T *t)
        : TypedScopePtr(t ? t->customScopePtr() : TypedScopePtr {nullptr, table<decayed_t<T>>})
    {
    }

    template <typename T>
    requires (!has_function_customScopePtr_v<T>)
    TypedScopePtr(T *t)
        : mScope(t)
        , mType(table<decayed_t<T>>)
    {
    }

    constexpr TypedScopePtr(const TypedScopePtr &other)
        : mScope(other.mScope)
        , mType(other.mType)
    {
    }

    TypedScopePtr(void *scope, const MetaTable *type)
        : mScope(scope)
        , mType(type)
    {
    }

    bool operator==(const TypedScopePtr &other) const
    {
        return mScope == other.mScope && mType == other.mType;
    }

    auto operator<=>(const TypedScopePtr &other) const = default;

    template <typename T>
    T *safe_cast() const
    {
        size_t offset = 0;
        if (!mType->isDerivedFrom<std::remove_const_t<T>>(&offset))
            std::terminate();

        return static_cast<T *>(reinterpret_cast<void*>(reinterpret_cast<char*>(mScope) + offset));
    }

    explicit operator bool() const
    {
        return mScope != nullptr;
    }

    ScopeIterator find(std::string_view key) const;
    ScopeField operator[](std::string_view key) const;
    //bool isEditable(const std::string &key) const;
    ScopeIterator begin() const;
    ScopeIterator end() const;

    std::string name() const;

    void call(ValueType &retVal, const ArgumentList &args) const;

    void *mScope = nullptr;
    const MetaTable * mType = nullptr;
};

}