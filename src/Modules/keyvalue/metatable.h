#pragma once

namespace Engine {

struct MODULES_EXPORT MetaTable {
    const char *mTypeName;
    const MetaTable &(*mBaseGetter)();
    const std::pair<const char *, Accessor> *mMember;

    ScopeIterator find(const std::string &key, TypedScopePtr scope) const;
    /*std::optional<ValueType> get(const std::string &key, TypedScopePtr scope) const;
    void set(const std::string &key, const ValueType &value, TypedScopePtr scope) const;
    bool isEditable(const std::string &key) const;*/

    template <typename T>
    bool isDerivedFrom() const;
    bool isDerivedFrom(const MetaTable *baseType) const;

	std::string name(TypedScopePtr scope) const;
};

}

DLL_IMPORT_VARIABLE2(const Engine::MetaTable, table, typename T);

template <typename T>
bool Engine::MetaTable::isDerivedFrom() const
{
    static_assert(std::is_base_of_v<ScopeBase, T>, "Only can check ScopeBase types");
    return isDerivedFrom(&table<T>());
}