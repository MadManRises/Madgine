#include "behaviorlib.h"

#include "nativebehaviorcollector.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"

#include "behavior.h"

DEFINE_UNIQUE_COMPONENT(Engine, NativeBehavior)

DEFINE_BEHAVIOR_FACTORY(Native, Engine::NativeBehaviorFactory)

namespace Engine {

std::vector<std::string_view> NativeBehaviorFactory::names() const
{
    const auto &names = kvKeys(NativeBehaviorRegistry::sComponentsByName());
    return std::vector<std::string_view> { names.begin(), names.end() };
}

Behavior NativeBehaviorFactory::create(std::string_view name, const ParameterTuple &args) const
{
    return NativeBehaviorRegistry::get(NativeBehaviorRegistry::sComponentsByName().at(name)).mInfo->create(args);
}

Threading::TaskFuture<ParameterTuple> NativeBehaviorFactory::createParameters(std::string_view name) const
{
    return NativeBehaviorRegistry::get(NativeBehaviorRegistry::sComponentsByName().at(name)).mInfo->createParameters();
}

std::vector<ValueTypeDesc> NativeBehaviorFactory::parameterTypes(std::string_view name) const
{
    auto types = NativeBehaviorRegistry::get(NativeBehaviorRegistry::sComponentsByName().at(name)).mInfo->parameterTypes();
    return { types.begin(), types.end() };
}

std::vector<ValueTypeDesc> NativeBehaviorFactory::resultTypes(std::string_view name) const
{
    auto types = NativeBehaviorRegistry::get(NativeBehaviorRegistry::sComponentsByName().at(name)).mInfo->resultTypes();
    return { types.begin(), types.end() };
}

}
