#include "../moduleslib.h"

#include "typedscopeptr.h"
#include "metatable.h"
#include "scopeiterator.h"

namespace Engine {

    std::optional<ValueType> TypedScopePtr::get(const std::string &key) const
    {
        return mType->get(key, *this);
    }

    ScopeIterator TypedScopePtr::find(const std::string &key) const
    {
        return mType->find(key, *this);
    }

    ScopeIterator TypedScopePtr::begin() const
    {
        return { *this, mType->mMember };
    }

    ScopeIterator TypedScopePtr::end() const
    {
        const std::pair<const char *, Accessor> *p = mType->mMember;
        while (p->first)
            ++p;
        return { *this, p};
    }

}