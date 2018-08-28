#include "../interfaceslib.h"

#include "pluginmanager.h"

namespace Engine
{
	namespace Plugins
	{
		PluginManager *PluginManager::sSingleton = nullptr;

		PluginManager& PluginManager::getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		PluginManager::PluginManager(const std::string &project) :
			mProject(project)			
		{
			assert(!sSingleton);
			sSingleton = this;
		}

		const std::string& PluginManager::project() const
		{
			return mProject;
		}

		PluginSection& PluginManager::operator[](const std::string& name)
		{
			return mSections.try_emplace(name, *this, name).first->second;
		}

		const PluginSection& PluginManager::at(const std::string& name) const
		{
			return mSections.at(name);
		}

		std::map<std::string, PluginSection>::const_iterator PluginManager::begin() const
		{
			return mSections.begin();
		}

		std::map<std::string, PluginSection>::const_iterator PluginManager::end() const
		{
			return mSections.end();
		}

		PluginSection::PluginSection(PluginManager& mgr, const std::string& name) :
		mMgr(mgr),
		mName(name)
		{
		}

		Plugin & PluginSection::getPlugin(const std::string & name)
		{
			auto pairIB = mPlugins.try_emplace(name, "Plugin_" + mMgr.project() + "_" + mName + "_" + name + CONFIG_SUFFIX);
			return pairIB.first->second;
		}

		std::map<std::string, Plugin>::const_iterator PluginSection::begin() const
		{
			return mPlugins.begin();
		}

		std::map<std::string, Plugin>::const_iterator PluginSection::end() const
		{
			return mPlugins.end();
		}

		void* PluginSection::getUniqueSymbol(const std::string& name) const
		{
			void *symbol = nullptr;
			for (const std::pair<const std::string, Plugin> &p : mPlugins)
			{
				void *s = p.second.getSymbol(name);
				if (s)
				{
					if (symbol)
						throw 0;
					symbol = s;
				}			
			}
			return symbol;
		}
	}
}
