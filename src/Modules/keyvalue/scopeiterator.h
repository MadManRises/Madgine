#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT ScopeIterator {

    ScopeIterator(TypedScopePtr scope, const std::pair<const char *, Accessor> *pointer);

    bool operator==(const ScopeIterator &other) const;

    bool operator!=(const ScopeIterator &other) const;

    std::pair<const char *, ValueType> operator*() const;

    void operator++();

private:
    void checkDerived();

    TypedScopePtr mScope;
    const std::pair<const char *, Accessor> *mPointer;
};

}