#pragma once

#include "Madgine/uniquecomponentcollector.h"

namespace Engine
{
	namespace Tools
	{
		
		using ToolsCollector = UniqueComponentCollector<ToolBase, std::vector, ImGuiRoot &>;

	}

	PLUGIN_COLLECTOR_EXPORT(Tools, Tools::ToolsCollector)

}