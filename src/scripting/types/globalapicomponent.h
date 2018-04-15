#pragma once

#include "../../uniquecomponent.h"
#include "globalapicomponentbase.h"
#include "scope.h"
#include "../../uniquecomponentcollector.h"

namespace Engine
{
	namespace Scripting
	{
		using GlobalAPICollector = BaseUniqueComponentCollector<GlobalAPIComponentBase, std::vector, GlobalScopeBase &>;

		template <class T>
		using GlobalAPIComponent = Scope<T, UniqueComponent<T, GlobalAPICollector>>;
	}
}
