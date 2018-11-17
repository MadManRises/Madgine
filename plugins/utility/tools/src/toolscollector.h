#pragma once

#include "Madgine/uniquecomponent/uniquecomponentcollector.h"

#include "toolbase.h"

namespace Engine
{
	namespace Tools
	{
		
		using ToolsCollector = UniqueComponentCollector<ToolBase, ImGuiRoot &>;

		using ToolsContainer = UniqueComponentContainer<ToolBase, ImGuiRoot&>;

	}

}

RegisterCollector(Engine::Tools::ToolsCollector);