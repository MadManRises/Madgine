#pragma once

#include "Madgine/uniquecomponentcollector.h"

namespace Engine
{
	namespace Tools
	{
		
		template <class Base, class... _Ty>
		class ToolsCreatorStore
		{
		public:
			static std::vector<Collector_F<Base, _Ty...>>& sComponents() {
				static std::vector<Collector_F<Base, _Ty...>> dummy;
				return dummy;
			}

			static constexpr size_t baseIndex() { return 0; }
		};

		using ToolsCollector = UniqueComponentCollector<ToolBase, ToolsCreatorStore<ToolBase, ImGuiRoot&>, std::vector, ImGuiRoot &>;

	}

	PLUGIN_COLLECTOR_EXPORT(Tools, Tools::ToolsCollector)

}