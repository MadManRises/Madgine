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

			Plugin &getPlugin(const std::string &name);

			Plugin &addPlugin(const std::string &name, const std::experimental::filesystem::path &path);

			std::map<std::string, Plugin>::const_iterator begin() const;
			std::map<std::string, Plugin>::const_iterator end() const;

		private:
			std::map<std::string, Plugin> mPlugins;

		};

	}
}
