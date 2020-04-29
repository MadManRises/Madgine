#include "../moduleslib.h"

#include "scopeiterator.h"

#include "accessor.h"

#include "valuetype.h"

#include "scopefield.h"

namespace Engine {

ScopeIterator::ScopeIterator(TypedScopePtr scope, const std::pair<const char *, Accessor> *pointer)
    : mScope(scope)
    , mCurrentTable(scope.mType)
    , mPointer(pointer)
{
    if (mPointer) {
        checkDerived();
    }
}

bool ScopeIterator::operator==(const ScopeIterator &other) const
{
    assert(mScope == other.mScope);
    if (mPointer == other.mPointer)
        return true;
    if (!mPointer)
        return !other.mPointer->first;
    else
        return !mPointer->first;
}

bool ScopeIterator::operator!=(const ScopeIterator &other) const
{
    assert(mScope.mScope == other.mScope.mScope);
    if (mPointer == other.mPointer)
        return false;
    if (!mPointer)
        return other.mPointer->first;
    else
        return mPointer->first;
}

ScopeField ScopeIterator::operator*() const
{
    return { mScope, mPointer };
}

Proxy<ScopeField> ScopeIterator::operator->() const
{
    return { mScope, mPointer };
}

void ScopeIterator::operator++()
{
    assert(mPointer);
    ++mPointer;
    checkDerived();
}

void ScopeIterator::checkDerived()
{
    while (!mPointer->first && mCurrentTable->mBase) {
        mCurrentTable = *mCurrentTable->mBase;
        mPointer = mCurrentTable->mMember;
    }
}

}