#pragma once

#include "accessor.h"
#include "valuetype.h"

namespace Engine {

struct ScopeIterator {

    ScopeIterator(TypedScopePtr scope, const std::pair<const char *, Accessor> *pointer)
        : mScope(scope)
        , mPointer(pointer)
    {
        if (mPointer) {
            checkDerived();
        }
    }

    bool operator==(const ScopeIterator &other) const
    {
        assert(mScope == other.mScope);
        if (mPointer == other.mPointer)
            return true;
        if (!mPointer)
            return !other.mPointer->first;
        else
            return !mPointer->first;
    }

    bool operator!=(const ScopeIterator &other) const
    {
        assert(mScope.mScope == other.mScope.mScope);
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
        assert(mPointer);
        ++mPointer;
        checkDerived();
    }

private:
    void checkDerived()
    {
        while (!mPointer->first && mScope.mType->mBaseGetter) {
            mScope.mType = &mScope.mType->mBaseGetter();
            mPointer = mScope.mType->mMember;
        }
    }

    TypedScopePtr mScope;
    const std::pair<const char *, Accessor> *mPointer;
};

}