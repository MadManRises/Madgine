#pragma once

#include "behaviorcollector.h"

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, NativeBehavior, DummyType<BehaviorStateBase>, Engine::UniqueComponent::FactoryImpl<Behavior>)

namespace Engine {

template <auto f>
struct NativeBehavior : NativeBehaviorComponent<NativeBehavior<f>> {
    
    static constexpr Behavior (*factory)() = f;
};

struct NativeBehaviorFactory : BehaviorListComponent<NativeBehaviorFactory> {
    static std::vector<std::string_view> names();
    static Behavior create(std::string_view name);
};

}

DECLARE_BEHAVIOR_FACTORY(Engine::NativeBehaviorFactory)
REGISTER_TYPE(Engine::DummyType<Engine::BehaviorStateBase>)

#define DECLARE_NATIVE_BEHAVIOR(Behavior) \
    REGISTER_TYPE(Engine::NativeBehavior<Behavior>)

#define DEFINE_NATIVE_BEHAVIOR(Behavior) \
    NAMED_UNIQUECOMPONENT(Behavior, Engine::NativeBehavior<Behavior>)
