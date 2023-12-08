#pragma once

#include "behaviorcollector.h"

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, NativeBehavior, DummyType<BehaviorStateBase>, Engine::UniqueComponent::FactoryImpl<Behavior>)

namespace Engine {

template <auto f>
struct NativeBehavior : NativeBehaviorComponent<NativeBehavior<f>> {
    using helper = Behavior (*)();
    operator helper()
    {
        return f;
    }
};

}

DECLARE_BEHAVIOR_FACTORY(Engine::NativeBehaviorRegistry)
REGISTER_TYPE(Engine::DummyType<Engine::BehaviorStateBase>)

#define DECLARE_NATIVE_BEHAVIOR(Behavior) \
    REGISTER_TYPE(Engine::NativeBehavior<Behavior>)

#define DEFINE_NATIVE_BEHAVIOR(Behavior) \
    NAMED_UNIQUECOMPONENT(Behavior, Engine::NativeBehavior<Behavior>)
