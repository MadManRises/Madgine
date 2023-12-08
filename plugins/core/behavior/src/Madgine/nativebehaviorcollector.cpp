#include "behaviorlib.h"

#include "nativebehaviorcollector.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "behavior.h"

DEFINE_UNIQUE_COMPONENT(Engine, NativeBehavior)

DEFINE_BEHAVIOR_FACTORY(Engine::NativeBehaviorRegistry)



