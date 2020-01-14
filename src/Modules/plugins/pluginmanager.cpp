#include "../moduleslib.h"

#if ENABLE_PLUGINS

#include "pluginmanager.h"

#include "plugin.h"

#include "../uniquecomponent/uniquecomponentregistry.h"

#include "Interfaces/stringutil.h"

#include "../generic/templates.h"

#include "Interfaces/filesystem/runtime.h"

#include "../keyvalue/keyvalue.h"

#include "Interfaces/filesystem/api.h"

#include "../threading/defaulttaskqueue.h"

#include "../cli/parameter.h"

#include "../ini/inisection.h"

#include "pluginsection.h"

namespace Engine {
namespace Plugins {

    CLI::Parameter<bool> noPluginCache { { "--no-plugin-cache", "-npc" }, false, "Disables the loading of the cached plugin selection at startup." };
    CLI::Parameter<std::string> loadPlugins { { "--load-plugins", "-lp" }, "", "If set the pluginmanager will load the specified config file after loading the cached plugin-file." };
    CLI::Parameter<std::string> exportPlugins { { "--export-plugins", "-ep" }, "", "If set the pluginmanager will save the current plugin selection after the boot to the specified config file and will export a corresponding uniquecomponent configuration source file." };

    PluginManager *PluginManager::sSingleton = nullptr;

    PluginManager &PluginManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    PluginManager::PluginManager()
    {
        assert(!sSingleton);
        sSingleton = this;

        const std::regex e { SHARED_LIB_PREFIX "Plugin_[a-zA-Z]*_([a-zA-Z]*)_[a-zA-Z]*\\" SHARED_LIB_SUFFIX };
        std::smatch match;
        for (auto path : Filesystem::listSharedLibraries()) {
            if (std::regex_match(path.str(), match, e)) {
                std::string section = match[1];
                mSections.try_emplace(section, *this, section);
            }
        }
    }

    PluginManager::~PluginManager()
    {

        for (PluginSection &sec : kvValues(mSections)) {
            for (Plugin &p : kvValues(sec)) {
                Plugin::LoadState state = p.unload();
                assert(state == Plugin::UNLOADED);
            }
        }
    }

    void PluginManager::executeCLI()
    {
        if (!loadPlugins->empty()) {
            Ini::IniFile file { Filesystem::Path { *loadPlugins } };
            LOG("Loading Plugins from '" << loadPlugins << "'");
            loadSelection(file);
        }

        if (!exportPlugins->empty()) {
            Threading::DefaultTaskQueue::getSingleton().update(0);

            Filesystem::Path p = *exportPlugins;
            Ini::IniFile file { p };
            LOG("Saving Plugins to '" << exportPlugins << "'");
            saveSelection(file, true);

            Filesystem::Path exportPath = p.parentPath() / ("components_" + p.stem() + ".cpp");

            exportStaticComponentHeader(exportPath, true);
        }
    }

    bool PluginManager::isLoaded(const std::string &plugin) const
    {
        for (const std::pair<const std::string, PluginSection> &sec : mSections) {
            if (sec.second.isLoaded(plugin))
                return true;
        }
        return false;
    }

    PluginSection &PluginManager::section(const std::string &name)
    {
        auto pib = mSections.try_emplace(name, *this, name);
        if (pib.second) {
            for (PluginListener *listener : mListeners) {
                setupListenerOnSectionAdded(listener, &pib.first->second);
            }
        }
        return pib.first->second;
    }

    PluginSection &PluginManager::operator[](const std::string &name)
    {
        return section(name);
    }

    const PluginSection &PluginManager::at(const std::string &name) const
    {
        return mSections.at(name);
    }

    Plugin *PluginManager::getPlugin(const std::string &name)
    {
        for (PluginSection &sec : kvValues(mSections)) {
            Plugin *p = sec.getPlugin(name);
            if (p)
                return p;
        }
        return nullptr;
    }

    std::map<std::string, PluginSection>::const_iterator PluginManager::begin() const
    {
        return mSections.begin();
    }

    std::map<std::string, PluginSection>::const_iterator PluginManager::end() const
    {
        return mSections.end();
    }

    std::map<std::string, PluginSection>::iterator PluginManager::begin()
    {
        return mSections.begin();
    }

    std::map<std::string, PluginSection>::iterator PluginManager::end()
    {
        return mSections.end();
    }

    void PluginManager::saveSelection(Ini::IniFile &file, bool withTools)
    {
        file.clear();
        for (auto &[name, section] : mSections) {
            if (!withTools && name == "Tools")
                continue;
            Ini::IniSection &iniSec = file[name];
            for (const std::pair<const std::string, Plugin> &p : section) {
                iniSec[p.first] = p.second.isLoaded() ? "On" : "";
            }
        }
        file.saveToDisk();
    }

    void PluginManager::loadSelection(Ini::IniFile &file)
    {
        if (mLoadingSelectionFile)
            return;
        file.loadFromDisk();
        mLoadingSelectionFile = true;
        for (auto &[name, section] : file) {
            (*this)[name].loadFromIni(section);
        }
        mLoadingSelectionFile = false;        
    }

    void PluginManager::addListener(PluginListener *listener)
    {
        {
            std::unique_lock lock(mListenersMutex);
            mListeners.push_back(listener);
        }
        for (PluginSection &section : kvValues(mSections))
            setupListenerOnSectionAdded(listener, &section);
    }

    void PluginManager::removeListener(PluginListener *listener)
    {
        {
            std::unique_lock lock(mListenersMutex);
            mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
        }
        for (PluginSection &section : kvValues(mSections))
            shutdownListenerAboutToRemoveSection(listener, &section);
    }

    void PluginManager::setupListenerOnSectionAdded(PluginListener *listener, PluginSection *section)
    {
        section->addListener(listener);
    }

    void PluginManager::shutdownListenerAboutToRemoveSection(PluginListener *listener, PluginSection *section)
    {
        section->removeListener(listener);
    }
}
}

#endif