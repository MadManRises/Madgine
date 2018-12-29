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
	MADGINE_TOOLS_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<Tools::ToolBase, Tools::ImGuiRoot&>;
#endif



}

RegisterCollector(Engine::Tools::ToolsCollector);