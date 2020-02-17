#include "../moduleslib.h"

#include "scopeiterator.h"
#include "typedscopeptr.h"
#include "valuetype.h"
#include "scopefield.h"

namespace Engine {

ScopeField TypedScopePtr::operator[](const std::string &key) const
{
    return *find(key);
}

/*bool TypedScopePtr::isEditable(const std::string &key) const
{
    return (*find(key)).isEditable();
}*/

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
    return { *this, nullptr };
}

std::string TypedScopePtr::name() const
{
    if (mScope)
        return mType->name(*this);
    else
        return "<NULL>";
}

}