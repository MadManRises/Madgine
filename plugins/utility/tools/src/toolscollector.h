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


#ifndef STATIC_BUILD
	template struct MADGINE_TOOLS_EXPORT UniqueComponentCollector<Tools::ToolBase, Tools::ImGuiRoot&>::Registry;
#endif



}

RegisterCollector(Engine::Tools::ToolsCollector);