#pragma once

#include "../generic/proxy.h"
#include "scopefield.h"

namespace Engine {

struct MODULES_EXPORT ScopeIterator {

    ScopeIterator(TypedScopePtr scope, const std::pair<const char *, Accessor> *pointer);

    bool operator==(const ScopeIterator &other) const;

    bool operator!=(const ScopeIterator &other) const;

    ScopeField operator*() const;
    Proxy<ScopeField> operator->() const;

    void operator++();

private:
    void checkDerived();

    TypedScopePtr mScope;
    const MetaTable *mCurrentTable;
    const std::pair<const char *, Accessor> *mPointer;
};

}