#pragma once

#include "../../uniquecomponent.h"
#include "scope.h"
#include "globalapicomponentcollector.h"
#include "globalapicomponentbase.h"

namespace Engine
{

	namespace Scripting
	{
		template <class T>
		using GlobalAPIComponent = Scope<T, UniqueComponent<T, GlobalAPICollector>>;
	}	

}
