#pragma once

namespace Engine {

    using Constructor = OwnedScopePtr(*)();

struct META_EXPORT MetaTable {
    const MetaTable **mSelf;
    const char *mTypeName;
    const MetaTable **mBase;    
    const std::pair<const char *, Accessor> *mMember;
    const Constructor *mConstructors;

    ScopeIterator find(std::string_view key, TypedScopePtr scope) const;

    void call(TypedScopePtr scope, ValueType &retVal, const ArgumentList &args) const;

    template <typename T>
    bool isDerivedFrom(size_t *offset = nullptr) const;
    bool isDerivedFrom(const MetaTable *baseType, size_t *offset = nullptr) const;

	std::string name(TypedScopePtr scope) const;
};

}

DLL_IMPORT_VARIABLE(const Engine::MetaTable, table, typename);

template <typename T>
bool Engine::MetaTable::isDerivedFrom(size_t *offset) const
{
    return isDerivedFrom(table<T>, offset);
}