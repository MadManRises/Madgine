#pragma once

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"

namespace Engine {
struct BehaviorFactoryList {
    template <typename T>
    BehaviorFactoryList(type_holder_t<T>)
        : names(T::names)
        , create(T::create)
    {
    }

    std::vector<std::string_view> (*names)();
    Behavior (*create)(std::string_view name);
};

template <typename T>
struct DummyType {
};
}

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, BehaviorList, DummyType<BehaviorFactoryList>, BehaviorFactoryList)


REGISTER_TYPE(Engine::BehaviorFactoryList)
REGISTER_TYPE(Engine::DummyType<Engine::BehaviorFactoryList>)

#define DECLARE_BEHAVIOR_FACTORY(Factory) \
    REGISTER_TYPE(Factory)

#define DEFINE_BEHAVIOR_FACTORY(Name, Factory) \
    NAMED_UNIQUECOMPONENT(Name, Factory)


