#pragma once

#include "plugin.h"

namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT PluginManager
		{
		public:
			PluginManager();

		private:
			std::map<std::string, Plugin> mPlugins;

		};

	}
}
