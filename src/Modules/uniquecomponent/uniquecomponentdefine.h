#pragma once 

#include "uniquecomponentcollector.h"
#include "uniquecomponent.h"
#include "uniquecomponentcontainer.h"


//base is included in __VA_ARGS__ to circumvent the problem with empty __VA_ARGS__ and ,
#define DECLARE_UNIQUE_COMPONENT(ns, prefix, macro_prefix, /*base,*/ ...) \
\
namespace ns \
{ \
	using prefix ## Collector = Engine::UniqueComponentCollector<__VA_ARGS__>; \
	template <template <typename ...> typename C> \
	using prefix ## Container = Engine::UniqueComponentContainer<C, __VA_ARGS__>; \
	using prefix ## Selector = Engine::UniqueComponentSelector<__VA_ARGS__>; \
	template <typename T> \
	using prefix ## Component = Engine::UniqueComponent<T, prefix ## Collector>; \
	template <typename T> \
	using prefix ## VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix ## Collector>; \
} \
\
namespace Engine \
{ \
	using namespace ns; \
	macro_prefix##_TEMPLATE_INSTANTIATION struct macro_prefix##_EXPORT UniqueComponentRegistry<__VA_ARGS__>; \
    } 

#define DEFINE_UNIQUE_COMPONENT(ns, prefix) RegisterType(ns::prefix ## Collector::Registry);