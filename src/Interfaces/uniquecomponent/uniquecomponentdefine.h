#pragma once 

#include "uniquecomponentcollector.h"
#include "uniquecomponentdefine.h"
#include "uniquecomponent.h"
#include "uniquecomponentcontainer.h"


#define DEFINE_UNIQUE_COMPONENT(ns, base, arg, prefix, macro_prefix) \
\
namespace ns \
{ \
	using prefix ## Collector = Engine::UniqueComponentCollector<base, arg>; \
	template <template <typename ...> typename C> \
	using prefix ## Container = Engine::UniqueComponentContainer<base, arg, C>; \
	using prefix ## Selector = Engine::UniqueComponentSelector<base, arg>; \
	template <typename T> \
	using prefix ## Component = Engine::UniqueComponent<T, prefix ## Collector>; \
	template <typename T> \
	using prefix ## VirtualBase = Engine::VirtualUniqueComponentBase<T, prefix ## Collector>; \
} \
\
namespace Engine \
{ \
	using namespace ns; \
	macro_prefix ## _TEMPLATE_INSTANTIATION struct macro_prefix ## _EXPORT UniqueComponentRegistry<base, arg>; \
} \
\
RegisterType(ns::prefix ## Collector::Registry);