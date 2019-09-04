#pragma once

namespace Engine {

struct MODULES_EXPORT MetaTable {
    const char *mName;
    const std::pair<const char *, Accessor> *mMember;

    ScopeIterator find(const std::string &key, TypedScopePtr scope) const;
    std::optional<ValueType> get(const std::string &key, TypedScopePtr scope) const;
    void set(const std::string &key, const ValueType &value, TypedScopePtr scope) const;
    bool isEditable(const std::string &key) const;

    template <typename T>
    bool isDerivedFrom() const;
    bool isDerivedFrom(const MetaTable *baseType) const;
};

}

DLL_IMPORT_VARIABLE2(const Engine::MetaTable, table, typename T);

template <typename X>
struct is_template : std::false_type {
};

template <template <class T, class _Data, template <typename> typename ResourceKind> class X, typename A, typename B, template <typename> typename C>
struct is_template<X<A, B, C>> : std::true_type {
};

template <typename T>
bool Engine::MetaTable::isDerivedFrom() const
{
    static_assert(std::is_base_of_v<ScopeBase, T>, "Only can check ScopeBase types");
    static_assert(!is_template<T>::value);
    return isDerivedFrom(&table<T>());
}