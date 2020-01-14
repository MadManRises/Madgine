#pragma once

#if ENABLE_PLUGINS

#include "../ini/inifile.h"

namespace Engine {
namespace Plugins {

    struct MODULES_EXPORT PluginManager {
        static PluginManager &getSingleton();

        PluginManager();
        ~PluginManager();

		void executeCLI();

        bool isLoaded(const std::string &plugin) const;

        PluginSection &section(const std::string &name);
        PluginSection &operator[](const std::string &name);
        const PluginSection &at(const std::string &name) const;

        Plugin *getPlugin(const std::string &name);

        std::map<std::string, PluginSection>::const_iterator begin() const;
        std::map<std::string, PluginSection>::const_iterator end() const;
        std::map<std::string, PluginSection>::iterator begin();
        std::map<std::string, PluginSection>::iterator end();

        void saveSelection(Ini::IniFile &file, bool withTools);
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

        static PluginManager *sSingleton;

        bool mLoadingSelectionFile = false;
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
