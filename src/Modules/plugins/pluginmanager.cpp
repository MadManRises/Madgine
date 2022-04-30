#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginmanager.h"

#    include "plugin.h"

#    include "Interfaces/dl/runtime.h"

#    include "Generic/keyvalue.h"

#    include "ini/inisection.h"

#    include "pluginsection.h"

#    include "ini/inifile.h"

#    include "../threading/workgroup.h"

#    include "Interfaces/filesystem/api.h"

namespace Engine {
namespace Plugins {

    static Filesystem::Path cacheFileName()
    {
        return Filesystem::appDataPath() / ("plugins.cfg");
    }

    static PluginManager *sSingleton = nullptr;

    PluginManager &PluginManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    PluginManager::PluginManager()
    {
        assert(!sSingleton);
        sSingleton = this;

        PLUGIN_GROUP_DEFINITIONS;

        const std::regex e { SHARED_LIB_PREFIX "Plugin_[a-zA-Z0-9]*_([a-zA-Z0-9]*)_[a-zA-Z0-9]*\\" SHARED_LIB_SUFFIX };
        std::smatch match;
        for (auto result : Dl::listSharedLibraries()) {
            auto file = result.path().filename();
            if (std::regex_match(file.str(), match, e)) {
                std::string section = match[1];
                mSections.try_emplace(section, *this, section);
            }
        }
    }

    bool PluginManager::setup(bool loadCache, std::string_view programName, std::string_view configFile)
    {
        mUseCache = loadCache;

        assert(Threading::WorkGroup::self().singleThreaded());

        if (loadCache || !configFile.empty()) {
            Filesystem::Path pluginFile = !configFile.empty() ? Filesystem::Path { configFile } : cacheFileName();

            Ini::IniFile file;
            if (file.loadFromDisk(pluginFile)) {
                LOG("Loading Plugins from '" << pluginFile << "'");
                bool result = loadSelection(file, true);
                if (!result)
                    return false;
            } else {
                if (!configFile.empty()) {
                    LOG_ERROR("Failed to open plugin-file '" << pluginFile << "'!");
                    return false;
                }
            }
        }

        if (!programName.empty()) {
            Plugin exe {"MadgineLauncher", nullptr, {}, "" };
            exe.setLoaded(true);
            exe.loadDependencies(*this);
            exe.clearDependencies(*this);
        }

        for (PluginSection &sec : kvValues(mSections)) {
            sec.load();
        }

        onUpdate();

        return true;
    }

    PluginManager::~PluginManager()
    {
        assert(Threading::WorkGroup::self().singleThreaded());

        /* for (PluginSection &sec : kvValues(mSections)) {
            if (sec.unload())
                throw 0;
        }*/
    }

    PluginSection &PluginManager::section(const std::string &name)
    {
        auto pib = mSections.try_emplace(name, *this, name);
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
            if (!withTools && StringUtil::endsWith(name, "Tools"))
                continue;
            Ini::IniSection &iniSec = file[name];
            for (std::pair<const std::string, Plugin> &p : section) {
                iniSec[p.first] = p.second.isLoaded() ? "On" : "";
            }
        }
    }

    bool PluginManager::loadSelection(const Ini::IniFile &file, bool withTools)
    {

        for (auto &[name, section] : file) {
            for (const std::pair<const std::string, std::string> &p : section) {
                Plugin *plugin = getPlugin(p.first);
                if (!plugin) {
                    LOG("Could not find Plugin \"" << p.first << "\"!");
                    continue;
                }
                plugin->setLoaded(!p.second.empty());
                if (withTools) {
                    Plugin *toolsPlugin = getPlugin(p.first + "Tools");
                    if (toolsPlugin) {
                        toolsPlugin->setLoaded(!p.second.empty());
                    }
                }
            }
        }

        for (PluginSection &sec : kvValues(mSections)) {
            sec.loadAllDependencies();
        }

        return true;
    }

    void PluginManager::onUpdate()
    {
        if (mUseCache) {
            Ini::IniFile file;
            saveSelection(file, false);
            file.saveToDisk(cacheFileName());
        }
    }

    bool PluginManager::loadFromFile(const Filesystem::Path &p, bool withTools)
    {
        LOG("Loading Plugins: " << p);
        if (!Filesystem::exists(p))
            return false;
        Ini::IniFile file;
        if (!file.loadFromDisk(p)) {
            return false;
        }
        return loadSelection(file, withTools);
    }

    void PluginManager::saveToFile(const Filesystem::Path &p, bool withTools)
    {
        LOG("Writing Plugins: " << p);
        Ini::IniFile file;
        saveSelection(file, withTools);
        file.saveToDisk(p);
    }

    void PluginManager::setupSection(const std::string &name, bool exclusive, bool atleastOne)
    {
        auto pib = mSections.try_emplace(name, *this, name, exclusive, atleastOne);
        assert(pib.second);
    }

}
}

#endif