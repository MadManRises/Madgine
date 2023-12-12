#include "behaviorlib.h"

#include "nativebehaviorcollector.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"

#include "behavior.h"

DEFINE_UNIQUE_COMPONENT(Engine, NativeBehavior)

DEFINE_BEHAVIOR_FACTORY(Native, Engine::NativeBehaviorFactory)

namespace Engine {

std::vector<std::string_view> NativeBehaviorFactory::names()
{
    const auto &names = kvKeys(NativeBehaviorRegistry::sComponentsByName());
    return std::vector<std::string_view> { names.begin(), names.end() };
}

Behavior NativeBehaviorFactory::create(std::string_view name)
{
    return NativeBehaviorRegistry::get(NativeBehaviorRegistry::sComponentsByName().at(name)).create();
}

}
