#pragma once

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"

namespace Engine {
struct BehaviorFactoryList {
    template <typename T>
    BehaviorFactoryList(type_holder_t<T>)
        : sComponentsByName(&T::Registry::sComponentsByName)
        , create([](uint32_t index) {
            return T::Registry::get(index).create();
        })
    {
    }

    const std::map<std::string_view, IndexType<uint32_t>> &(*sComponentsByName)();
    Behavior (*create)(uint32_t index);
};

template <typename T>
struct DummyType {
};
}

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, BehaviorList, DummyType<BehaviorFactoryList>, BehaviorFactoryList)

namespace Engine {

template <typename _Registry>
struct BehaviorFactory : BehaviorListComponent<BehaviorFactory<_Registry>> {
    using Registry = _Registry;
};

}

REGISTER_TYPE(Engine::BehaviorFactoryList)
REGISTER_TYPE(Engine::DummyType<Engine::BehaviorFactoryList>)

#define DECLARE_BEHAVIOR_FACTORY(Registry) \
    REGISTER_TYPE(Engine::BehaviorFactory<Registry>)

#define DEFINE_BEHAVIOR_FACTORY(Registry) \
    NAMED_UNIQUECOMPONENT(Native, Engine::BehaviorFactory<Registry>)


