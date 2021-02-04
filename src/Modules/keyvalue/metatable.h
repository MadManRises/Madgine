#pragma once

namespace Engine {

struct MODULES_EXPORT MetaTable {
    const MetaTable **mSelf;
    const char *mTypeName;
    const MetaTable **mBase;
    const std::pair<const char *, Accessor> *mMember;

    ScopeIterator find(const std::string &key, TypedScopePtr scope) const;

    template <typename T>
    bool isDerivedFrom() const;
    bool isDerivedFrom(const MetaTable *baseType) const;

	std::string name(TypedScopePtr scope) const;
};

}

DLL_IMPORT_VARIABLE(const Engine::MetaTable, table, typename);

template <typename T>
bool Engine::MetaTable::isDerivedFrom() const
{
    //static_assert(std::is_base_of_v<ScopeBase, T>, "Only can check ScopeBase types");
    return isDerivedFrom(table<T>);
}