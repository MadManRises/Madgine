#include "../metalib.h"

#include "ownedscopeptr.h"

#include "proxyscopebase.h"

namespace Engine {

OwnedScopePtr::OwnedScopePtr(std::shared_ptr<ProxyScopeBase> ptr)
    : mScope(std::move(ptr))
{
}

std::string OwnedScopePtr::name() const
{
    return get().name();
}

bool OwnedScopePtr::operator==(const OwnedScopePtr &other) const
{
    return mScope == other.mScope;
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