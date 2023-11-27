#pragma once

#include "table_forward.h"

namespace Engine {

using Constructor = OwnedScopePtr (*)();

struct META_EXPORT MetaTable {

    constexpr MetaTable(const MetaTable **self, const char *name, const std::pair<const char *, Accessor> *members, const Constructor *constructors = nullptr)
        : mSelf(self)
        , mTypeName(name)
        , mBase(nullptr)
        , mBaseOffset(nullptr)
        , mMembers(members)
        , mConstructors(constructors)
    {
    }

    template <typename T, typename Base>
    constexpr MetaTable(const char *name, type_holder_t<T>, type_holder_t<Base>, const std::pair<const char *, Accessor> *members, const Constructor *constructors)
        : mSelf(&table<T>)
        , mTypeName(name)
        , mBase(&table<Base>)
        , mBaseOffset(&Engine::inheritance_offset<Base, T>)
        , mMembers(members)
        , mConstructors(constructors)
    {
    }

    template <typename T>
    constexpr MetaTable(const char *name, type_holder_t<T>, type_holder_t<void>, const std::pair<const char *, Accessor> *members, const Constructor *constructors)
        : mSelf(&table<T>)
        , mTypeName(name)
        , mBase(nullptr)
        , mBaseOffset(nullptr)
        , mMembers(members)
        , mConstructors(constructors)
    {
    }

    ScopeIterator find(std::string_view key, TypedScopePtr scope) const;

    void call(TypedScopePtr scope, ValueType &retVal, const ArgumentList &args) const;

    template <typename T>
    bool isDerivedFrom(size_t *offset = nullptr) const
    {
        return isDerivedFrom(table<T>, offset);
    }
    bool isDerivedFrom(const MetaTable *baseType, size_t *offset = nullptr) const;

    std::string name(TypedScopePtr scope) const;

    const MetaTable **mSelf;
    const char *mTypeName;
    const MetaTable **mBase;
    size_t (*mBaseOffset)();
    const std::pair<const char *, Accessor> *mMembers;
    const Constructor *mConstructors;
};

}
