#pragma once

#include "uniquecomponentcollector.h"
#include "uniquecomponentcontainer.h"

//base is included in __VA_ARGS__ to circumvent the problem with empty __VA_ARGS__ and ,
#define DECLARE_UNIQUE_COMPONENT(ns, prefix, /*base, */...)                                       \
    namespace ns {                                                                                \
    using prefix##Registry = Engine::UniqueComponentRegistry<__VA_ARGS__>;                        \
    using prefix##Collector = Engine::UniqueComponentCollector<prefix##Registry, __VA_ARGS__>;    \
    template <typename C>                                                                         \
    using prefix##Container = Engine::UniqueComponentContainer<C, prefix##Registry, __VA_ARGS__>; \
    using prefix##Selector = Engine::UniqueComponentSelector<prefix##Registry, __VA_ARGS__>;      \
    template <typename T>                                                                         \
    using prefix##Component = Engine::UniqueComponent<T, prefix##Collector>;                      \
    template <typename T>                                                                         \
    using prefix##VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix##Collector>;         \
    }

#if defined(STATIC_BUILD)
#    define EXPORT_REGISTRY(Registry)
#else
#    define EXPORT_REGISTRY(Registry) DLL_EXPORT_VARIABLE2(, Registry, Engine::, uniqueComponentRegistry, {}, Registry)
#endif

#define DEFINE_UNIQUE_COMPONENT(ns, prefix)        \
    RegisterType(ns::prefix##Registry); \
    EXPORT_REGISTRY(ns::prefix##Registry)