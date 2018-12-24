#pragma once

#include "../ini/inifile.h"

#include "plugin.h"

namespace Engine {
	namespace Plugins {

		class INTERFACES_EXPORT PluginSection
		{
		public:
			PluginSection(PluginManager &mgr, const std::string &name, const std::set<std::string> &fixedPlugins = {});

			void setAtleastOne(bool atleastOne = true);
			bool isAtleastOne() const;
			void setExclusive(bool exclusive = true);
			bool isExclusive() const;

			bool isLoaded(const std::string &name) const;
			bool loadPlugin(const std::string &name);
			bool unloadPlugin(const std::string &name);

			void addListener(PluginListener *listener);
			void removeListener(PluginListener *listener);

			template <class T>
			T *getUniqueSymbol(const std::string &name) const {
				return static_cast<T*>(getUniqueSymbol(name));
			}
			const void *getUniqueSymbol(const std::string &name) const;

			std::map<std::string, Plugin>::const_iterator begin() const;
			std::map<std::string, Plugin>::const_iterator end() const;

			void loadFromIni(Ini::IniSection &sec);

		private:
			Plugin *getPlugin(const std::string &name);

			bool loadPlugin(Plugin *p);
			bool unloadPlugin(Plugin *p);

			std::map<std::string, Plugin> mPlugins;
			std::vector<PluginListener *> mListeners;

			bool mAtleastOne = false;
			bool mExclusive = false;

			std::string mName;

			PluginManager &mMgr;
		};


	}
}