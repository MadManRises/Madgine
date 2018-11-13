#pragma once

#include "plugin.h"

#include "../ini/inifile.h"

namespace Engine
{
	namespace Plugins
	{

		struct PluginSectionListener {
			virtual void onSectionAdded(PluginSection *sec) {}
			virtual void aboutToRemoveSection(PluginSection *sec) {}
		};

		struct PluginListener {
			virtual bool aboutToUnloadPlugin(const Plugin *p) { return true; }
			virtual bool aboutToLoadPlugin(const Plugin *p) { return true; }
			virtual void onPluginUnload(const Plugin *p) {}
			virtual void onPluginLoad(const Plugin *p) {}
		};

		class INTERFACES_EXPORT PluginSection
		{
		public:
			PluginSection(PluginManager &mgr, const std::string &name);

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
			void *getUniqueSymbol(const std::string &name) const;

			std::map<std::string, Plugin>::const_iterator begin() const;
			std::map<std::string, Plugin>::const_iterator end() const;

			void loadFromIni(Ini::IniSection &sec);

		private:
			Plugin *getPlugin(const std::string &name);

			std::map<std::string, Plugin> mPlugins;
			std::vector<PluginListener *> mListeners;

			bool mAtleastOne = false;
			bool mExclusive = false;

			std::string mName;

			PluginManager &mMgr;
		};



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
			std::experimental::filesystem::path currentSelectionPath();
			const std::string &currentSelectionName();
			void setCurrentSelection(const std::string &key, const std::experimental::filesystem::path &path);

			void addListener(PluginSectionListener *listener);
			void removeListener(PluginSectionListener *listener);

		private:
			std::map<std::string, PluginSection> mSections;

			std::vector<PluginSectionListener*> mListeners;

			std::string mProject;

			Ini::IniFile mSettings;

			std::optional<Ini::IniFile> mCurrentSelectionFile;

			static PluginManager *sSingleton;

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
