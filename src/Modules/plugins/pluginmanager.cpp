#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginmanager.h"

#    include "plugin.h"

#    include "../uniquecomponent/uniquecomponentregistry.h"

#    include "Interfaces/filesystem/runtime.h"

#    include "../keyvalue/keyvalue.h"

#    include "../threading/defaulttaskqueue.h"

#    include "../cli/parameter.h"

#    include "../ini/inisection.h"

#    include "pluginsection.h"

#    include "../ini/inifile.h"

#    include "../threading/workgroup.h"

#    include "../threading/barrier.h"

#    include "Interfaces/filesystem/api.h"

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

    PluginManager::PluginManager(const std::string &programName, bool loadCache, bool loadExe)
        : mCacheFileName(programName + ".cfg")
    {
        assert(!sSingleton);
        sSingleton = this;

        PLUGIN_GROUP_DEFINITIONS;

        const std::regex e { SHARED_LIB_PREFIX "Plugin_[a-zA-Z0-9]*_([a-zA-Z0-9]*)_[a-zA-Z0-9]*\\" SHARED_LIB_SUFFIX };
        std::smatch match;
        for (auto path : Filesystem::listSharedLibraries()) {
            if (std::regex_match(path.str(), match, e)) {
                std::string section = match[1];
                mSections.try_emplace(section, *this, section);
            }
        }

        assert(Threading::WorkGroup::self().singleThreaded());

        Threading::Barrier barrier { Threading::Barrier::NO_FLAGS, 1 };

        std::promise<bool> p1;
        Future<bool> f1 { p1.get_future() };

        if (loadCache) {
            barrier.queue(nullptr, [this, &barrier, p { std::move(p1) }]() mutable {
                std::string pluginFile = !loadPlugins->empty() ? *loadPlugins : mCacheFileName;

                Ini::IniFile file;
                if (file.loadFromDisk(pluginFile)) {
                    LOG("Loading Plugins from '" << pluginFile << "'");
                    loadSelection(barrier, file, std::move(p));
                } else {
                    if (!loadPlugins->empty())
                        LOG_ERROR("Failed to open plugin-file '" << pluginFile << "'!");
                }
            });
        } else {
            f1 = true;
        }

        Future<bool> f2;

        if (loadExe) {
            std::promise<bool> p2;
            f2 = p2.get_future();
            barrier.queue(nullptr, [this, &barrier, f { std::move(f1) }, p { std::move(p2) }]() mutable {
                if (!f.isAvailable())
                    return Threading::YIELD;
                Plugin exe { "MadgineLauncher", nullptr, {}, "" };
                exe.load(*this, barrier, std::move(p));
                exe.clearDependencies(*this);
                return Threading::RETURN;
            });
        } else {
            f2 = std::move(f1);
        }

        if (!exportPlugins->empty()) {
            barrier.queue(nullptr, [this, &barrier, f { std::move(f2) }]() mutable {
                if (!f.isAvailable())
                    return Threading::YIELD;
                Filesystem::Path p = *exportPlugins;
                Ini::IniFile file;
                LOG("Saving Plugins to '" << exportPlugins << "'");
                saveSelection(barrier, file, true);
                file.saveToDisk(p);

                Filesystem::Path exportPath = p.parentPath() / ("components_" + p.stem() + ".cpp");

                exportStaticComponentHeader(exportPath, true); //TODO Consider using a signal to remove dependency plugin->uniquecomponent
                return Threading::RETURN;
            });
        }

        Threading::TaskQueue queue { "PluginManager" };
        barrier.enter(&queue, 0, true);
    }

    PluginManager::~PluginManager()
    {
        assert(Threading::WorkGroup::self().singleThreaded());

        Threading::Barrier barrier { Threading::Barrier::NO_FLAGS, 1 };

        std::vector<Future<bool>> results;

        for (PluginSection &sec : kvValues(mSections)) {
            results.emplace_back(sec.unload(barrier));
        }

        Threading::TaskQueue queue { "PluginManager" };
        barrier.enter(&queue, 0, true);

        for (Future<bool> &result : results) {
            if (!result.isAvailable() || result)
                throw 0;
        }
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

    void PluginManager::saveSelection(Threading::Barrier &barrier, Ini::IniFile &file, bool withTools)
    {
        file.clear();
        for (auto &[name, section] : mSections) {
            if (!withTools && name == "Tools")
                continue;
            Ini::IniSection &iniSec = file[name];
            for (std::pair<const std::string, Plugin> &p : section) {
                iniSec[p.first] = p.second.isLoaded() ? "On" : "";
            }
        }
    }

    void PluginManager::loadSelection(Threading::Barrier &barrier, const Ini::IniFile &file, std::promise<bool> &&p)
    {
        assert(!mLoadingSelectionFile);
        mLoadingSelectionFile = true;

        std::vector<Future<bool>> futures;

        for (auto &[name, section] : file) {
            futures.emplace_back((*this)[name].loadFromIni(barrier, section));
        }
        mLoadingSelectionFile = false;

        barrier.queue(nullptr, [this, &barrier, futures { std::move(futures) }, p { std::move(p) }]() mutable {
            bool wait = false;
            for (Future<bool> &f : futures) {
                if (!f.isAvailable())
                    wait = true;
                else if (!f) {
                    p.set_value(false);
                    return Threading::RETURN;
                }
            }
            if (wait)
                return Threading::YIELD;
            onUpdate(barrier);
            p.set_value(true);
            return Threading::RETURN;
        });
    }

    void PluginManager::onUpdate(Threading::Barrier &barrier)
    {
        Ini::IniFile file;
        saveSelection(barrier, file, false);
        file.saveToDisk(mCacheFileName);
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

    Future<bool> PluginManager::loadFromFile(const Filesystem::Path &p)
    {
        if (!Filesystem::exists(p))
            return false;
        std::promise<bool> promise;
        Future<bool> result { promise.get_future() };
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        barrier.queue(nullptr, [=, &barrier, promise { std::move(promise) }]() mutable {
            Ini::IniFile file;
            if (!file.loadFromDisk(p)) {
                promise.set_value(false);
                return;
            }
            loadSelection(barrier, file, std::move(promise));
        });
        return result;
    }

    void PluginManager::saveToFile(const Filesystem::Path &p, bool withTools)
    {
        Threading::Barrier &barrier = Threading::WorkGroup::barrier();
        barrier.queue(nullptr, [=, &barrier]() {
            Ini::IniFile file;
            saveSelection(barrier, file, withTools);
            file.saveToDisk(p);
        });
    }

    void PluginManager::setupListenerOnSectionAdded(PluginListener *listener, PluginSection *section)
    {
        section->addListener(listener);
    }

    void PluginManager::shutdownListenerAboutToRemoveSection(PluginListener *listener, PluginSection *section)
    {
        section->removeListener(listener);
    }

    void PluginManager::setupSection(const std::string &name, bool exclusive, bool atleastOne)
    {
        auto pib = mSections.try_emplace(name, *this, name, exclusive, atleastOne);
        assert(pib.second);
        for (PluginListener *listener : mListeners) {
            setupListenerOnSectionAdded(listener, &pib.first->second);
        }
    }
}
}

#endif