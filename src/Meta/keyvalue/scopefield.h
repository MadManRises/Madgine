#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct META_EXPORT ScopeField {

    ScopeField(const TypedScopePtr &ptr, const std::pair<const char *, Accessor> *pointer);

    void value(ValueType &retVal) const;

    ScopeField &operator=(const ValueType &v);

    const char *key() const;

    bool isEditable() const;
    bool isGeneric() const;

private:
    TypedScopePtr mScope;
    const std::pair<const char *, Accessor> *mPointer;
};

}