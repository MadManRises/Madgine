#pragma once

#if ENABLE_PLUGINS

#include "../ini/inifile.h"

#include "pluginsection.h"

namespace Engine {
namespace Plugins {

    class MODULES_EXPORT PluginManager {
    public:
        static PluginManager &getSingleton();

        PluginManager();
        ~PluginManager();

		void executeCLI();

        bool isLoaded(const std::string &plugin) const;

        const std::string &project() const;

        PluginSection &section(const std::string &name);
        PluginSection &operator[](const std::string &name);
        const PluginSection &at(const std::string &name) const;

        Plugin *getPlugin(const std::string &name);

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

        void saveSelection(Ini::IniFile &file);
        void loadSelection(Ini::IniFile &file);

        void addListener(PluginListener *listener);
        void removeListener(PluginListener *listener);

        std::mutex mListenersMutex;

    protected:
        void setupListenerOnSectionAdded(PluginListener *listener, PluginSection *section);
        void shutdownListenerAboutToRemoveSection(PluginListener *listener, PluginSection *section);

    private:
        std::map<std::string, PluginSection> mSections;

        std::vector<PluginListener *> mListeners;

        std::string mProject;

        Ini::IniFile mSettings;

        std::optional<Ini::IniFile> mCurrentSelectionFile;

        static PluginManager *sSingleton;

        bool mLoadingCurrentSelectionFile = false;
    };

}

}

#define IF_PLUGIN(p) if (Engine::Plugins::PluginManager::getSingleton().isLoaded(#p))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#else

#include "Interfaces/macros.h"

#define IF_PLUGIN(p) if constexpr (!Engine::streq("BUILD_" #p, STRINGIFY2(BUILD_##p)))
#define THROW_PLUGIN(errorMsg) throw errorMsg

#endif
