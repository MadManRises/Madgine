#pragma once


namespace Engine {

struct MetaTable {
    const char *mName;
    const std::pair<const char *, Accessor> *mMember;

    ScopeIterator find(const std::string &key, ScopeBase *scope) const;
    std::optional<ValueType> get(const std::string &key, ScopeBase *scope) const;
};

}


DLL_IMPORT_VARIABLE2(const Engine::MetaTable, table, typename T);