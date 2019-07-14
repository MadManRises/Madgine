#pragma once

#include "accessor.h"
#include "valuetype.h"

namespace Engine {

struct ScopeIterator {

    ScopeIterator(ScopeBase *scope, const std::pair<const char *, Accessor> *pointer)
        : mScope(scope)
        , mPointer(pointer)
    {
    }

    bool operator==(const ScopeIterator &other) const
    {
        assert(mScope == other.mScope);
        return mPointer == other.mPointer;
    }

    bool operator!=(const ScopeIterator &other) const
    {
        assert(mScope == other.mScope);
        if (mPointer == other.mPointer)
            return false;
        if (!mPointer)
            return other.mPointer->first;
        else
            return mPointer->first;
    }

    std::pair<const char *, ValueType> operator*() const
    {
        return { mPointer->first, mPointer->second.mGetter(mScope) };
    }

    void operator++()
    {
        ++mPointer;
    }

private:
    ScopeBase *mScope;
    const std::pair<const char *, Accessor> *mPointer;
};

}