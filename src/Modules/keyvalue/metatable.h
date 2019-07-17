#pragma once

namespace Engine {

struct MetaTable {
    const char *mName;
    const std::pair<const char *, Accessor> *mMember;

    ScopeIterator find(const std::string &key, TypedScopePtr scope) const;
    std::optional<ValueType> get(const std::string &key, TypedScopePtr scope) const;

    template <typename T>
    bool isInstance() const
    {
        static_assert(std::is_base_of_v<ScopeBase, T>, "Only can check ScopeBase types");
        return this == &table<T>();
    }
};

}

DLL_IMPORT_VARIABLE2(const Engine::MetaTable, table, typename T);