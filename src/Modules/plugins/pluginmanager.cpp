#include "../moduleslib.h"

#if ENABLE_PLUGINS

#include "pluginmanager.h"

#include "plugin.h"

#include "Interfaces/stringutil.h"

#include "../generic/templates.h"

#include "Interfaces/filesystem/runtime.h"

#include "../keyvalue/keyvalue.h"

#include "../cli/cli.h"

namespace Engine {
namespace Plugins {

    CLI::Parameter<bool> noPluginCache{ { "--no-plugin-cache", "-npc" }, false, "Disables the loading of the cached plugin selection at startup." };
    CLI::Parameter<std::string> loadPlugins{ { "--load-plugins", "-lp" }, "", "If set the pluginmanager will load the specified config file after loading the cached plugin-file." };
    CLI::Parameter<std::string> savePlugins{ { "--save-plugins", "-sp" }, "", "If set the pluginmanager will save the current plugin selection after the boot to the specified config file." };

    PluginManager *PluginManager::sSingleton = nullptr;

    PluginManager &PluginManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

	PluginManager::PluginManager()
		: mSettings("Madgine_plugins.cfg")
	{
		assert(!sSingleton);
		sSingleton = this;

		const std::regex e{ SHARED_LIB_PREFIX "Plugin_[a-zA-Z]*_([a-zA-Z]*)_[a-zA-Z]*\\" SHARED_LIB_SUFFIX };
		std::smatch match;
		for (auto path : Filesystem::listSharedLibraries()) {
			if (std::regex_match(path.str(), match, e)) {
				std::string section = match[1];
				mSections.try_emplace(section, *this, section);
			}
		}

		std::string currentSelection = mSettings["State"]["CurrentSelectionFile"];

		if (!currentSelection.empty() && !noPluginCache) {
			Filesystem::Path p = selectionFiles()[currentSelection];
			p /= currentSelection + ".cfg";
			mCurrentSelectionFile = Ini::IniFile(p);
			loadCurrentSelectionFile();
		}

		if (!loadPlugins->empty()) {
			Ini::IniFile file{ Filesystem::Path{ *loadPlugins } };
			LOG("Loading Plugins from '" << loadPlugins << "'");
			loadSelection(file);
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

    void PluginManager::saveCurrentSelectionFile()
    {
        if (mCurrentSelectionFile && !mLoadingCurrentSelectionFile) {
            saveSelection(*mCurrentSelectionFile);
        }
    }

    void PluginManager::loadCurrentSelectionFile()
    {
        assert(!mLoadingCurrentSelectionFile);
        if (mCurrentSelectionFile) {
            loadSelection(*mCurrentSelectionFile);
        }
    }

    Ini::IniSection &PluginManager::selectionFiles()
    {
        return mSettings["SelectionFiles"];
    }

    Filesystem::Path PluginManager::currentSelectionPath()
    {
        return mCurrentSelectionFile ? mCurrentSelectionFile->path().parentPath() : "";
    }

    const std::string &PluginManager::currentSelectionName()
    {
        return mSettings["State"]["CurrentSelectionFile"];
    }

    void PluginManager::setCurrentSelection(const std::string &key, const Filesystem::Path &path)
    {
        mSettings["State"]["CurrentSelectionFile"] = key;
        selectionFiles()[key] = path.str();
        mSettings.saveToDisk();
        mCurrentSelectionFile = Ini::IniFile(path / (key + ".cfg"));
        loadCurrentSelectionFile();
    }

    void PluginManager::saveSelection(Ini::IniFile &file)
    {
        file.clear();
        for (auto &[name, section] : mSections) {
            Ini::IniSection &iniSec = file[name];
            for (const std::pair<const std::string, Plugin> &p : section) {
                iniSec[p.first] = p.second.isLoaded() ? "On" : "";
            }
        }
        file.saveToDisk();
    }

    void PluginManager::loadSelection(Ini::IniFile &file)
    {
        file.loadFromDisk();
        mLoadingCurrentSelectionFile = true;
        for (auto &[name, section] : file) {
            if (name == "Core")
                continue;
            (*this)[name].loadFromIni(section);
        }
        mLoadingCurrentSelectionFile = false;
        saveCurrentSelectionFile();
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