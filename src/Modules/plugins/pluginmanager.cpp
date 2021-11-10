#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginmanager.h"

#    include "plugin.h"

#    include "Interfaces/dl/runtime.h"

#    include "Generic/keyvalue.h"

#    include "cli/parameter.h"

#    include "ini/inisection.h"

#    include "pluginsection.h"

#    include "ini/inifile.h"

#    include "../threading/workgroup.h"

#    include "../threading/barrier.h"

#    include "Interfaces/filesystem/api.h"

namespace Engine {
namespace Plugins {

    CLI::Parameter<bool> noPluginCache { { "--no-plugin-cache", "-npc" }, false, "Disables the loading of the cached plugin selection at startup." };
    CLI::Parameter<std::string> loadPlugins { { "--load-plugins", "-lp" }, "", "If set the pluginmanager will load the specified config file after loading the cached plugin-file." };
    CLI::Parameter<std::string> exportPlugins { { "--export-plugins", "-ep" }, "", "If set the pluginmanager will save the current plugin selection after the boot to the specified config file and will export a corresponding uniquecomponent configuration source file." };

    static Filesystem::Path cacheFileName()
    {
        return Filesystem::appDataPath() / ("plugins.cfg");
    }

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

    bool PluginManager::setup(bool loadCache, bool loadExe)
    {
        assert(Threading::WorkGroup::self().singleThreaded());

        Threading::Barrier barrier { Threading::Barrier::NO_FLAGS, 1 };

        Threading::TaskFuture<bool> result = barrier.queue(nullptr, [](PluginManager *_this, Threading::Barrier &barrier, bool loadCache, bool loadExe) mutable -> Threading::Task<bool> {
            if (loadCache && (!noPluginCache || !loadPlugins->empty())) {
                Filesystem::Path pluginFile = !loadPlugins->empty() ? Filesystem::Path { *loadPlugins } : cacheFileName();

                Ini::IniFile file;
                if (file.loadFromDisk(pluginFile)) {
                    LOG("Loading Plugins from '" << pluginFile << "'");
                    bool result = co_await _this->loadSelection(barrier, file);
                } else {
                    if (!loadPlugins->empty())
                        LOG_ERROR("Failed to open plugin-file '" << pluginFile << "'!");
                    co_return false;
                }
            }

            if (loadExe) {
                Plugin exe { "MadgineLauncher", nullptr, {}, "" };
                bool result = co_await exe.load(*_this, barrier);
                exe.clearDependencies(*_this);
            }

            if (!exportPlugins->empty()) {
                auto helper = [_this, &barrier](const Filesystem::Path &path, bool hasTools) {
                    Ini::IniFile file;
                    LOG("Saving Plugins to '" << path << "'");
                    _this->saveSelection(barrier, file, hasTools);
                    file.saveToDisk(path);

                    Filesystem::Path exportPath = path.parentPath() / ("components_" + std::string { path.stem() } + ".cpp");

                    _this->mExportSignal.emit(exportPath, hasTools);
                };

                Filesystem::Path p = *exportPlugins;
                helper(p, false);
                Filesystem::Path p_tools = p.parentPath() / (std::string { p.stem() } + "_tools" + std::string { p.extension() });
                helper(p_tools, true);
            }
            co_return true;
        }, this, barrier, loadCache, loadExe);

        Threading::TaskQueue queue { "PluginManager" };
        barrier.enter(&queue, 0, true);
        return result;
    }

    PluginManager::~PluginManager()
    {
        assert(Threading::WorkGroup::self().singleThreaded());

        Threading::Barrier barrier { Threading::Barrier::NO_FLAGS, 1 };

        std::vector<Threading::TaskFuture<bool>> results;

        for (PluginSection &sec : kvValues(mSections)) {
            results.emplace_back(sec.unload(barrier));
        }

        Threading::TaskQueue queue { "PluginManager" };
        barrier.enter(&queue, 0, true);

        for (Threading::TaskFuture<bool> &result : results) {
            if (!result.is_ready() || result)
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
            if (!withTools && StringUtil::endsWith(name, "Tools"))
                continue;
            Ini::IniSection &iniSec = file[name];
            for (std::pair<const std::string, Plugin> &p : section) {
                iniSec[p.first] = p.second.isLoaded() ? "On" : "";
            }
        }
    }

    Threading::TaskFuture<bool> PluginManager::loadSelection(Threading::Barrier &barrier, Ini::IniFile file)
    {
        return barrier.queue(nullptr, [](PluginManager *_this, Threading::Barrier &barrier, Ini::IniFile file) -> Threading::Task<bool> {
            assert(!_this->mLoadingSelectionFile);
            _this->mLoadingSelectionFile = true;

            std::vector<Threading::TaskFuture<bool>> futures;

            for (auto &[name, section] : file) {
                futures.emplace_back((*_this)[name].loadFromIni(barrier, section));
            }
            _this->mLoadingSelectionFile = false;

            for (Threading::TaskFuture<bool> &future : futures) {
                if (!co_await future) {
                    co_return false;
                }
            }
            _this->onUpdate(barrier);
            co_return true;
        }, this, barrier, std::move(file));
    }

    void PluginManager::onUpdate(Threading::Barrier &barrier)
    {
        if (!noPluginCache) {
            Ini::IniFile file;
            saveSelection(barrier, file, false);
            file.saveToDisk(cacheFileName());
        }
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

    Threading::TaskFuture<bool> PluginManager::loadFromFile(const Filesystem::Path &p)
    {
        LOG("Loading Plugins: " << p);
        if (!Filesystem::exists(p))
            return false;
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        Ini::IniFile file;
        if (!file.loadFromDisk(p)) {
            return false;
        }
        return loadSelection(barrier, file);
    }

    void PluginManager::saveToFile(const Filesystem::Path &p, bool withTools)
    {
        LOG("Writing Plugins: " << p);
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

    Threading::SignalStub<Filesystem::Path, bool> &PluginManager::exportSignal()
    {
        return mExportSignal;
    }
}
}

#endif