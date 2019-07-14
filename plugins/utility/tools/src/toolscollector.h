#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "toolbase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, ToolBase, ImGuiRoot&, Tools, MADGINE_TOOLS);

/*
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
*/