#include "../metalib.h"

#include "scopeiterator.h"
#include "typedscopeptr.h"
#include "scopefield.h"

#include "virtualscope.h" //enforce export of VirtualScopeBase<void>

namespace Engine {

ScopeField TypedScopePtr::operator[](std::string_view key) const
{
    return *find(key);
}

/*bool TypedScopePtr::isEditable(const std::string &key) const
{
    return (*find(key)).isEditable();
}*/

ScopeIterator TypedScopePtr::find(std::string_view key) const
{
    return mType->find(key, *this);
}

ScopeIterator TypedScopePtr::begin() const
{
    return { *this, mType ? mType->mMember : nullptr };
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

void TypedScopePtr::call(ValueType &retVal, const ArgumentList &args) const
{
    return mType->call(*this, retVal, args);
}

}