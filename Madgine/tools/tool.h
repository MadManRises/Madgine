#pragma once

#include "Madgine/uniquecomponent.h"

#include "toolscollector.h"

#include "toolbase.h"

namespace Engine
{
	namespace Tools
	{
		
		template <class T>
		using Tool = UniqueComponent<T, ToolsCollector>;

	}
}