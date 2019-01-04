#pragma once

#include "../ini/inifile.h"

#include "pluginsection.h"

namespace Engine
{
	namespace Plugins
	{


		class INTERFACES_EXPORT PluginManager
		{
		public:
			static PluginManager &getSingleton();

			PluginManager(const std::string &project);

			bool isLoaded(const std::string &plugin) const;

			const std::string &project() const;

			PluginSection &operator[](const std::string &name);
			const PluginSection &at(const std::string &name) const;

			std::map<std::string, PluginSection>::const_iterator begin() const;
			std::map<std::string, PluginSection>::const_iterator end() const;
			std::map<std::string, PluginSection>::iterator begin();
			std::map<std::string, PluginSection>::iterator end();

			void saveCurrentSelectionFile();
			void loadCurrentSelectionFile();

			Ini::IniSection &selectionFiles();
			Filesystem::Path currentSelectionPath();
			const std::string &currentSelectionName();
			void setCurrentSelection(const std::string &key, const Filesystem::Path &path);

			void addListener(PluginListener *listener);
			void removeListener(PluginListener *listener);

		protected:
			void setupListenerOnSectionAdded(PluginListener *listener, PluginSection *section);
			void shutdownListenerAboutToRemoveSection(PluginListener *listener, PluginSection *section);

			void setupCoreSection();

		private:
			std::map<std::string, PluginSection> mSections;

			std::vector<PluginListener*> mListeners;

			std::string mProject;

			Ini::IniFile mSettings;

			std::optional<Ini::IniFile> mCurrentSelectionFile;

			static PluginManager *sSingleton;

			bool mLoadingCurrentSelectionFile = false;

		};


	}

	constexpr bool streq(const char *lhs, const char *rhs)
	{
		int i = -1;
		do{
			++i;
			if (lhs[i] != rhs[i])
				return false;
		} while (lhs[i]);
		return true;
	}

}

#ifdef STATIC_BUILD
#define STRINGIFY(a) #a
#define STRINGIFY2(a) STRINGIFY(a)
#define IF_PLUGIN(p) if constexpr(!Engine::streq("BUILD_PLUGIN_" #p, STRINGIFY2(BUILD_PLUGIN_ ## p)))
#define THROW_PLUGIN(errorMsg) throw errorMsg
#else
#define IF_PLUGIN(p) if (Engine::Plugins::PluginManager::getSingleton().isLoaded(#p))
#define THROW_PLUGIN(errorMsg) throw errorMsg
#endif
