#pragma once

#include "uniquecomponent.h"
#include "uniquecomponentcollector.h"
#include "uniquecomponentcontainer.h"

//base is included in __VA_ARGS__ to circumvent the problem with empty __VA_ARGS__ and ,
#define DECLARE_UNIQUE_COMPONENT(ns, prefix, /*base,*/...)                                \
    namespace ns {                                                                        \
    using prefix##Collector = Engine::UniqueComponentCollector<__VA_ARGS__>;              \
    template <template <typename...> typename C>                                          \
    using prefix##Container = Engine::UniqueComponentContainer<C, __VA_ARGS__>;           \
    using prefix##Selector = Engine::UniqueComponentSelector<__VA_ARGS__>;                \
    template <typename T>                                                                 \
    using prefix##Component = Engine::UniqueComponent<T, prefix##Collector>;              \
    template <typename T>                                                                 \
    using prefix##VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix##Collector>; \
    }

#define DEFINE_UNIQUE_COMPONENT(ns, prefix) \
    RegisterType(ns::prefix##Collector::Registry); \
	DLL_EXPORT_VARIABLE2(, ns::prefix##Collector::Registry, Engine::, uniqueComponentRegistry, {}, ns::prefix##Collector::Registry)