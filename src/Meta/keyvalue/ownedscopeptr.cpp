#include "../metalib.h"

#include "ownedscopeptr.h"

#include "proxyscopebase.h"

namespace Engine {

std::string OwnedScopePtr::name() const
{
    return get().name();
}

bool OwnedScopePtr::operator==(const OwnedScopePtr &other) const
{
    return mScope == other.mScope;
}

OwnedScopePtr::operator TypedScopePtr() const {
    return get();
}

TypedScopePtr OwnedScopePtr::get() const
{
    return mScope->proxyScopePtr();
}

const MetaTable *OwnedScopePtr::type() const
{
    return get().mType;
}

}