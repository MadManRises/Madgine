#include "behaviorlib.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "behaviorcollector.h"

#include "behavior.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::BehaviorHandle)
CONSTRUCTOR()
MEMBER(mName)
METATABLE_END(Engine::BehaviorHandle)

DEFINE_UNIQUE_COMPONENT(Engine, BehaviorFactory)

namespace Engine {

Behavior BehaviorHandle::create(const ParameterTuple &args) const
{
    return BehaviorFactoryRegistry::get(mIndex).mFactory->create(mName, args);
}

Threading::TaskFuture<ParameterTuple> BehaviorHandle::createParameters() const
{
    return BehaviorFactoryRegistry::get(mIndex).mFactory->createParameters(mName);
}

std::vector<ValueTypeDesc> BehaviorHandle::parameterTypes() const
{
    return BehaviorFactoryRegistry::get(mIndex).mFactory->parameterTypes(mName);
}

std::vector<ValueTypeDesc> BehaviorHandle::resultTypes() const
{
    return BehaviorFactoryRegistry::get(mIndex).mFactory->resultTypes(mName);
}

std::string BehaviorHandle::toString() const
{
    return std::string { BehaviorFactoryRegistry::sComponentName(mIndex) } + "/" + mName;
}

bool BehaviorHandle::fromString(std::string_view s)
{
    size_t separator = s.find('/');
    if (separator == std::string_view::npos)
        return false;
    std::string_view category = s.substr(0, separator);
    std::string_view name = s.substr(separator + 1);

    auto it = BehaviorFactoryRegistry::sComponentsByName().find(category);

    if (it == BehaviorFactoryRegistry::sComponentsByName().end())
        return false;

    if (!std::ranges::contains(BehaviorFactoryRegistry::get(it->second).mFactory->names(), name))
        return false;

    mIndex = it->second;
    mName = name;

    return true;
}

BehaviorHandle::operator bool() const
{
    return static_cast<bool>(mIndex);
}

}
