#include "behaviorlib.h"

#include "behavior.h"

#include "Meta/keyvalue/valuetype.h"

#include "Meta/serialize/serializetable_impl.h"
#include "Meta/keyvalue/metatable_impl.h"

SERIALIZETABLE_BEGIN(Engine::Behavior)
SERIALIZETABLE_END(Engine::Behavior)

METATABLE_BEGIN(Engine::Behavior)
PROXY(mState)
METATABLE_END(Engine::Behavior)

METATABLE_BEGIN(Engine::BehaviorStateBase)
METATABLE_END(Engine::BehaviorStateBase)

namespace Engine {

Behavior::Behavior(std::unique_ptr<BehaviorStateBase> state)
    : mState(std::move(state))
{
}

Behavior &Behavior::operator=(std::unique_ptr<BehaviorStateBase> state)
{
    mState = std::move(state);
    return *this;
}

std::string_view Behavior::name() const
{
    return mState->name();
}

}