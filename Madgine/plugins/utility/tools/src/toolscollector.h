#pragma once

#include "Madgine/uniquecomponentcollector.h"

#include "toolbase.h"

namespace Engine
{
	namespace Tools
	{
		
		using ToolsCollector = UniqueComponentCollector<ToolBase, ImGuiRoot &>;

		using ToolsCollectorInstance = UniqueComponentCollectorInstance<ToolBase, ImGuiRoot&>;

	}

}

RegisterCollector(Engine::Tools::ToolsCollector);