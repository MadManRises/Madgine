#pragma once

#include "plugin.h"

namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT PluginManager
		{
		public:
			PluginManager(const std::string &selfDllName = "");

		private:
			std::map<std::string, Plugin> mPlugins;

		};

	}
}
