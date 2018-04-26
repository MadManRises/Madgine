#include "../interfaceslib.h"

#include "pluginmanager.h"

namespace Engine
{
	namespace Plugins
	{
		PluginManager::PluginManager(const std::string& selfDllName)
		{
			if (!selfDllName.empty())
			{
				assert(mPlugins.try_emplace(selfDllName, selfDllName).first->second.load());
			}
			assert(mPlugins.try_emplace("Self", "").first->second.load());
		}
	}
}
