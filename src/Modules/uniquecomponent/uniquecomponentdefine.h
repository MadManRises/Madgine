#pragma once 

#include "uniquecomponentcollector.h"
#include "uniquecomponent.h"
#include "uniquecomponentcontainer.h"



#define DECLARE_UNIQUE_COMPONENT(ns, base, prefix, macro_prefix, ...) \
\
namespace ns \
{ \
	using prefix ## Collector = Engine::UniqueComponentCollector<base, ## __VA_ARGS__>; \
	template <template <typename ...> typename C> \
	using prefix ## Container = Engine::UniqueComponentContainer<base, C, ## __VA_ARGS__>; \
	using prefix ## Selector = Engine::UniqueComponentSelector<base, ## __VA_ARGS__>; \
	template <typename T> \
	using prefix ## Component = Engine::UniqueComponent<T, prefix ## Collector>; \
	template <typename T> \
	using prefix ## VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix ## Collector>; \
} \
\
namespace Engine \
{ \
	using namespace ns; \
	macro_prefix##_TEMPLATE_INSTANTIATION struct macro_prefix##_EXPORT UniqueComponentRegistry<base, ## __VA_ARGS__>; \
    } 

#define DEFINE_UNIQUE_COMPONENT(ns, prefix) RegisterType(ns::prefix ## Collector::Registry);