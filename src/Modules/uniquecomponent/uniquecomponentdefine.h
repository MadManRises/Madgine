#pragma once

#include "Generic/replace.h"

//base is included in __VA_ARGS__ to circumvent the problem with empty __VA_ARGS__ and ,
#define DECLARE_UNIQUE_COMPONENT2(ns, prefix, registry, component, /*base, */...)                                                                                               \
    namespace ns {                                                                                                                                                              \
    using prefix##Registry = registry<__VA_ARGS__>;                                                                                                                             \
    using prefix##BaseRegistry = Engine::UniqueComponentRegistry<__VA_ARGS__>;                                                                                      \
    using prefix##Collector = Engine::UniqueComponentCollector<prefix##Registry, __VA_ARGS__>;                                                                                  \
    template <typename C>                                                                                                                                                       \
    using prefix##Container = Engine::UniqueComponentContainer<typename Engine::replace<C>::template type<std::unique_ptr<FIRST(__VA_ARGS__)>>, prefix##Registry, __VA_ARGS__>; \
    using prefix##Selector = Engine::UniqueComponentSelector<prefix##Registry, __VA_ARGS__>;                                                                          \
    template <typename T, typename Base = FIRST(__VA_ARGS__)>                                                                                                                   \
    using prefix##Component = component<Engine::UniqueComponent<T, prefix##Collector, Base>>;                                                                                   \
    template <typename T, typename Base = FIRST(__VA_ARGS__)>                                                                                                                   \
    using prefix##VirtualBase = component<Engine::VirtualUniqueComponentBase<T, prefix##Collector, Base>>;                                                                      \
    }                                                                                                                                                                           \
    RegisterType(ns::prefix##Registry);

#define DECLARE_UNIQUE_COMPONENT(ns, prefix, /*base, */...) DECLARE_UNIQUE_COMPONENT2(ns, prefix, Engine::UniqueComponentRegistry, std::type_identity_t, __VA_ARGS__)

#define DECLARE_NAMED_UNIQUE_COMPONENT(ns, prefix, /*base, */...)                                                          \
    DECLARE_UNIQUE_COMPONENT2(ns, prefix, Engine::NamedUniqueComponentRegistry, Engine::NamedUniqueComponent, __VA_ARGS__) \
    RegisterType(ns::prefix##BaseRegistry);

#if defined(STATIC_BUILD)
#    define EXPORT_REGISTRY(Registry, BaseRegistry)
#else
#    define EXPORT_REGISTRY(Registry, BaseRegistry) DLL_EXPORT_VARIABLE3_ORDER(, BaseRegistry, Registry, Engine::, uniqueComponentRegistry,, {}, BaseRegistry)
#endif

#define DEFINE_UNIQUE_COMPONENT(ns, prefix) \
    EXPORT_REGISTRY(ns::prefix##Registry, ns::prefix##BaseRegistry)
        