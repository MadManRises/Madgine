#pragma once

#include "Generic/replace.h"
#include "Generic/functor.h"

//base is included in __VA_ARGS__ to circumvent the problem with empty __VA_ARGS__ and ,
#define DECLARE_UNIQUE_COMPONENT(ns, prefix, /*base, */...)                                                                                                                     \
    namespace ns {                                                                                                                                                              \
    using prefix##Registry = Engine::UniqueComponentRegistry<__VA_ARGS__>;                                                                                                      \
    using prefix##Collector = Engine::UniqueComponentCollector<prefix##Registry, __VA_ARGS__>;                                                                                  \
    template <typename C>                                                                                                                                                       \
    using prefix##Container = Engine::UniqueComponentContainer<typename Engine::replace<C>::template type<std::unique_ptr<FIRST(__VA_ARGS__)>>, prefix##Registry, __VA_ARGS__>; \
    template <typename Observer = Engine::NoOpFunctor>                                                                                                                                  \
    using prefix##Selector = Engine::UniqueComponentSelector<Observer, prefix##Registry, __VA_ARGS__>;                                                                                    \
    template <typename T, typename Base = FIRST(__VA_ARGS__)>                                                                                                                   \
    using prefix##Component = Engine::UniqueComponent<T, prefix##Collector, Base>;                                                                                              \
    template <typename T, typename Base = FIRST(__VA_ARGS__)>                                                                                                                   \
    using prefix##VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix##Collector, Base>;                                                                                 \
    }                                                                                                                                                                           \
    RegisterType(ns::prefix##Registry);

#if defined(STATIC_BUILD)
#    define EXPORT_REGISTRY(Registry)
#else
#    define EXPORT_REGISTRY(Registry) DLL_EXPORT_VARIABLE2_ORDER(, Registry, Engine::, uniqueComponentRegistry, {}, Registry)
#endif

#define DEFINE_UNIQUE_COMPONENT(ns, prefix) \
    EXPORT_REGISTRY(ns::prefix##Registry)