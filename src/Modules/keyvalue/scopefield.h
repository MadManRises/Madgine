#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT ScopeField {

    ScopeField(const TypedScopePtr &ptr, const std::pair<const char *, Accessor> *pointer);

    ValueType value() const;

    operator ValueType() const;
    ScopeField &operator=(const ValueType &v);

    const char *key() const;

    bool isEditable() const;

private:
    TypedScopePtr mScope;
    const std::pair<const char *, Accessor> *mPointer;
};

}