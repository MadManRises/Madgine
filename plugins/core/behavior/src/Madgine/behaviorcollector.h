#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_NAMED_UNIQUE_COMPONENT(Engine, Behavior, BehaviorStateBase, VariableScope *)

namespace Engine {

template <typename T>
struct BehaviorState : VirtualScope<T, BehaviorComponent<T>> {
    using VirtualScope<T, BehaviorComponent<T>>::VirtualScope;

    virtual std::string_view name() const override
    {
        return T::componentName();
    }
};

}
