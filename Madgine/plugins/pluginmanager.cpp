#include "../interfaceslib.h"

#include "pluginmanager.h"

namespace Engine
{
	namespace Plugins
	{
		PluginManager::PluginManager()
		{
		}

		Plugin & PluginManager::addPlugin(const std::string & name, const std::experimental::filesystem::path & path)
		{
			auto pairIB = mPlugins.try_emplace(name, path);
			return pairIB.first->second;
		}

		std::map<std::string, Plugin>::const_iterator PluginManager::begin() const
		{
			return mPlugins.begin();
		}

		std::map<std::string, Plugin>::const_iterator PluginManager::end() const
		{
			return mPlugins.end();
		}

		Plugin & PluginManager::getPlugin(const std::string & name) {
			return mPlugins.at(name);
		}

	}
}
