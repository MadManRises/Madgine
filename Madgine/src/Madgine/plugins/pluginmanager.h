#pragma once

#include "plugin.h"

namespace Engine
{
	namespace Plugins
	{
		class INTERFACES_EXPORT PluginManager
		{
		public:
			PluginManager(const std::string &project, const std::string &type);

			Plugin &getPlugin(const std::string &name);

			std::map<std::string, Plugin>::const_iterator begin() const;
			std::map<std::string, Plugin>::const_iterator end() const;

			void *getUniqueSymbol(const std::string &name) const;

		private:
			std::map<std::string, Plugin> mPlugins;

			std::string mProject, mType;

		};

	}
}
