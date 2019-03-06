#pragma once

#include "Interfaces/uniquecomponent/uniquecomponent.h"

#include "toolscollector.h"

#include "toolbase.h"

namespace Engine
{
	namespace Tools
	{
		template <class T>
		struct __fixWeirdBug {
			typedef UniqueComponent<T, ToolsCollector> type;
		};

		template <class T>
		using Tool = typename __fixWeirdBug<T>::type;

		/*template <class T>
		using Tool = UniqueComponent<T, ToolsCollector>;*/

	}
}