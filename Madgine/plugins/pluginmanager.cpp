#include "../interfaceslib.h"

#include "pluginmanager.h"

namespace Engine
{
	namespace Plugins
	{
		PluginManager::PluginManager(const std::string &project, const std::string &type) :
			mProject(project),
			mType(type)
		{

		}

		Plugin & PluginManager::getPlugin(const std::string & name)
		{
			auto pairIB = mPlugins.try_emplace(name, "Plugin_" + mProject + "_" + mType + "_" + name + CONFIG_SUFFIX);
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

	}
}
