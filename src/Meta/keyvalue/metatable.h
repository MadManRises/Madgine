#pragma once

#include "table_forward.h"

namespace Engine {

using Constructor = OwnedScopePtr (*)();

struct META_EXPORT MetaTable {
    const MetaTable **mSelf;
    const char *mTypeName;
    const MetaTable **mBase;
    size_t (*mBaseOffset)();
    const std::pair<const char *, Accessor> *mMember;
    const Constructor *mConstructors;

    ScopeIterator find(std::string_view key, TypedScopePtr scope) const;

    void call(TypedScopePtr scope, ValueType &retVal, const ArgumentList &args) const;

    template <typename T>
    bool isDerivedFrom(size_t *offset = nullptr) const
    {
        return isDerivedFrom(table<T>, offset);
    }
    bool isDerivedFrom(const MetaTable *baseType, size_t *offset = nullptr) const;

    std::string name(TypedScopePtr scope) const;
};

}
