#pragma once

#include "plugin.h"

namespace Engine
{
	namespace Plugins
	{


		class INTERFACES_EXPORT PluginSection
		{
		public:
			PluginSection(PluginManager &mgr, const std::string &name);

			Plugin &getPlugin(const std::string &name);

			void *getUniqueSymbol(const std::string &name) const;

			std::map<std::string, Plugin>::const_iterator begin() const;
			std::map<std::string, Plugin>::const_iterator end() const;



		private:
			std::map<std::string, Plugin> mPlugins;

			std::string mName;

			PluginManager &mMgr;
		};



		class INTERFACES_EXPORT PluginManager
		{
		public:
			static PluginManager &getSingleton();

			PluginManager(const std::string &project);

			const std::string &project() const;

			PluginSection &operator[](const std::string &name);
			const PluginSection &at(const std::string &name) const;

			std::map<std::string, PluginSection>::const_iterator begin() const;
			std::map<std::string, PluginSection>::const_iterator end() const;

		private:
			std::map<std::string, PluginSection> mSections;

			std::string mProject;

			static PluginManager *sSingleton;

		};


	}
}
