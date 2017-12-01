#pragma once

#include "uniquecomponent.h"
#include "globalapicomponentbase.h"
#include "scripting/types/scope.h"
#include "uniquecomponentcollector.h"

namespace Engine
{
	namespace Scripting
	{
		using GlobalAPICollector = BaseUniqueComponentCollector<GlobalAPIComponentBase>;

		template <class T>
		using GlobalAPIComponent = Scope<T, UniqueComponent<T, GlobalAPICollector>>;
	}
}
