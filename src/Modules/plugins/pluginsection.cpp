#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginsection.h"

#    include "plugin.h"

#    include "pluginmanager.h"

#    include "Interfaces/exception.h"

#    include "../threading/defaulttaskqueue.h"

#    include "pluginlistener.h"

#    include "Interfaces/filesystem/runtime.h"

#    include "../keyvalue/keyvalue.h"

#    include "../ini/inisection.h"

#    include "../threading/workgroup.h"

#    include "../threading/barrier.h"

namespace Engine {
namespace Plugins {

    PluginSection::PluginSection(PluginManager &mgr, const std::string &name, bool exclusive, bool atleastOne)
        : mName(name)
        , mMgr(mgr)
        , mAtleastOne(atleastOne)
        , mExclusive(exclusive)
    {
        const std::regex e { SHARED_LIB_PREFIX "Plugin_([a-zA-Z0-9]*)_" + mName + "_([a-zA-Z0-9]*)\\" SHARED_LIB_SUFFIX };
        std::smatch match;
        for (auto path : Filesystem::listSharedLibraries()) {
            if (std::regex_match(path.str(), match, e)) {
                std::string project = match[1];
                std::string name = match[2];
                auto pib = mPlugins.try_emplace(name, name, this, project, path);
                assert(pib.second);
            }
        }
    }

    PluginSection::~PluginSection()
    {
        assert(mDependents.empty());
    }

    bool PluginSection::isAtleastOne() const
    {
        return mAtleastOne;
    }

    bool PluginSection::isExclusive() const
    {
        return mExclusive;
    }

    Future<bool> PluginSection::load(Threading::Barrier &barrier)
    {
        if (mAtleastOne) {
            if (mPlugins.empty())
                throw exception("No plugin available in Section tagged as atleastOne: "s + mName);
            for (std::pair<const std::string, Plugin> &p : mPlugins) {
                Future<bool> state = p.second.state(Plugin::LOADING);
                if (!state.isAvailable() || state)
                    return state;
            }
            /*if (!*/ return loadPlugin(barrier, &mPlugins.begin()->second); /*)
                throw exception("Failed to load default Plugin for atleastOne Section: "s + mPlugins.begin()->first);*/
        }
        return true;
    }

    Future<bool> PluginSection::unload(Threading::Barrier &barrier)
    {
        std::vector<Future<bool>> dependentsList;

        for (Plugin *p : mDependents) {
            dependentsList.emplace_back(p->section()->unloadPlugin(barrier, p));
        }

        std::promise<bool> promise;
        Future<bool> result = promise.get_future();

        barrier.queue(nullptr, [this, &barrier, dependentsList { std::move(dependentsList) }, promise { std::move(promise) }]() mutable {
            bool wait = false;
            for (Future<bool> &f : dependentsList) {
                if (!f.isAvailable()) {
                    wait = true;
                } else if (f) {
                    LOG_ERROR("Unload of dependent for plugin-section \"" << mName << "\" failed!");
                    return Threading::RETURN;
                }
            }
            if (wait) {
                return Threading::YIELD;
            } else {
                std::vector<Future<bool>> results;
                for (Plugin &p : kvValues(mPlugins)) {
                    results.emplace_back(unloadPlugin(barrier, &p));
                }
                barrier.queue(nullptr, [this, results { std::move(results) }, promise { std::move(promise) }]() mutable {
                    bool wait = false;
                    for (Future<bool> &f : results) {
                        if (!f.isAvailable()) {
                            wait = true;
                        } else if (f) {
                            LOG_ERROR("Unload of plugin in section \"" << mName << "\" failed!");
                            promise.set_value(true);
                            return Threading::RETURN;
                        }
                    }
                    if (wait) {
                        return Threading::YIELD;
                    } else {
                        promise.set_value(false);
                        return Threading::RETURN;
                    }
                });
                return Threading::RETURN;
            }
        });
        return result;
    }

    PluginSection::State PluginSection::isLoaded(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it != mPlugins.end()) {
            Future<bool> state = it->second.state();
            if (!state.isAvailable())
                return UNDEFINED;
            return state ? LOADED : UNLOADED;
        }
        return UNLOADED;
    }

    Future<bool> PluginSection::loadPlugin(const std::string &name)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        std::promise<bool> p;
        Future<bool> f { p.get_future() };
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        barrier.queue(nullptr, [=, &barrier, p { std::move(p) }, f { f.share() }]() mutable {
            loadPlugin(barrier, plugin, std::move(p), std::move(f));
            mMgr.onUpdate(barrier);
        });
        return f;
    }

    Future<bool> PluginSection::unloadPlugin(const std::string &name)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        std::promise<bool> p;
        Future<bool> f { p.get_future() };
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        barrier.queue(nullptr, [=, &barrier, p { std::move(p) }, f { f.share() }]() mutable {
            unloadPlugin(barrier, plugin, std::move(p), std::move(f));
            mMgr.onUpdate(barrier);
        });
        return f;
    }

    bool PluginSection::loadPluginByFilename(const std::string &name)
    {
        auto pib = mPlugins.try_emplace(name, name);
        assert(pib.second);
        return loadPlugin(name);
    }

    void PluginSection::addListener(PluginListener *listener)
    {
        std::unique_lock lock(mMgr.mListenersMutex);
        mListeners.emplace(listener);
        for (std::pair<const std::string, Plugins::Plugin> &p : *this) {
            if (p.second.state())
                listener->onPluginLoad(&p.second);
        }
    }

    void PluginSection::removeListener(PluginListener *listener)
    {
        std::unique_lock lock(mMgr.mListenersMutex);
        for (std::pair<const std::string, Plugins::Plugin> &p : *this) {
            if (p.second.state())
                listener->aboutToUnloadPlugin(&p.second);
        }
        mListeners.erase(listener);
    }

    Plugin *PluginSection::getPlugin(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it == mPlugins.end())
            return nullptr;
        return &it->second;
    }

    Future<bool> PluginSection::loadPlugin(Threading::Barrier &barrier, Plugin *p, std::optional<std::promise<bool>> &&promise, std::optional<Future<bool>> &&future)
    {
        assert(p->section() == this);

        Future<bool> result = p->startOperation(Plugin::LOADING, promise, std::move(future));

        if (promise) {
            std::vector<Future<void>> listenerFeedbacks;
            {
                std::unique_lock lock(mMgr.mListenersMutex);
                for (PluginListener *listener : mListeners)
                    listenerFeedbacks.emplace_back(listener->aboutToLoadPlugin(p));
            }

            barrier.queue(nullptr, [this, p, &barrier, promise { std::move(*promise) }, listenerFeedbacks { std::move(listenerFeedbacks) }]() mutable {
                for (Future<void> &f : listenerFeedbacks)
                    if (!f.isAvailable())
                        return Threading::YIELD;

                Future<bool> result = true;
                Plugin *unloadExclusive
                    = nullptr;
                if (mExclusive) {
                    for (Plugin &p2 : kvValues(mPlugins)) {
                        if (&p2 != p && p2.isLoaded()) {
                            assert(!unloadExclusive);
                            unloadExclusive = &p2;
                        }
                    }
                }

                if (unloadExclusive) {
                    result = unloadPlugin(barrier, unloadExclusive);
                }
                barrier.queue(nullptr, [this, unloadExclusive, p, &barrier, promise { std::move(promise) }, result { std::move(result) }]() mutable {
                    if (!result.isAvailable())
                        return Threading::YIELD;
                    if (unloadExclusive && result) {
                        promise.set_value(false);
                        return Threading::RETURN;
                    }

                    std::promise<bool> pluginPromise;
                    Future<bool> f { pluginPromise.get_future() };
                    p->load(mMgr, barrier, std::move(pluginPromise));
                    barrier.queue(nullptr, [this, p, &barrier, promise { std::move(promise) }, f { std::move(f) }]() mutable {
                        if (!f.isAvailable())
                            return Threading::YIELD;

                        if (!f) {
                            promise.set_value(false);
                            return Threading::RETURN;
                        }

                        {
                            std::unique_lock lock(mMgr.mListenersMutex);
                            for (PluginListener *listener : mListeners)
                                listener->onPluginLoad(p);
                        }

                        PluginSection &toolsSection = mMgr.section("Tools");
                        if (Plugin *toolPlugin = toolsSection.getPlugin(p->name() + "Tools")) {
                            toolsSection.loadPlugin(barrier, toolPlugin);
                        }
                        promise.set_value(true);

                        return Threading::RETURN;
                    });
                    return Threading::RETURN;
                });
                return Threading::RETURN;
            });
        }

        return result;
    }

    Future<bool> PluginSection::unloadPlugin(Threading::Barrier &barrier, Plugin *p, std::optional<std::promise<bool>> &&promise, std::optional<Future<bool>> &&future)
    {
        assert(p->section() == this);

        Future<bool> result = p->startOperation(Plugin::UNLOADING, promise, std::move(future));

        if (promise) {

            //assert(!mAtleastOne);
            std::vector<Future<void>> listenerFeedbacks;
            {
                std::unique_lock lock(mMgr.mListenersMutex);
                for (PluginListener *listener : mListeners)
                    listenerFeedbacks.emplace_back(listener->aboutToUnloadPlugin(p));
            }

            barrier.queue(nullptr, [this, p, &barrier, promise { std::move(*promise) }, listenerFeedbacks { std::move(listenerFeedbacks) }]() mutable {
                for (Future<void> &f : listenerFeedbacks)
                    if (!f.isAvailable())
                        return Threading::YIELD;
                std::promise<bool> pluginPromise;
                Future<bool> result { pluginPromise.get_future() };
                p->unload(mMgr, barrier, std::move(pluginPromise));
                barrier.queue(nullptr, [this, p, promise { std::move(promise) }, result { std::move(result) }]() mutable {
                    if (!result.isAvailable())
                        return Threading::YIELD;
                    bool resultValue = result;
                    if (resultValue) {
                        std::unique_lock lock(mMgr.mListenersMutex);
                        for (PluginListener *listener : mListeners)
                            listener->onPluginUnload(p);
                    }
                    promise.set_value(resultValue);
                    return Threading::RETURN;
                });
                return Threading::RETURN;
            });
        }

        return result;
    }

    std::map<std::string, Plugin>::const_iterator PluginSection::begin() const
    {
        return mPlugins.begin();
    }

    std::map<std::string, Plugin>::const_iterator PluginSection::end() const
    {
        return mPlugins.end();
    }

    std::map<std::string, Plugin>::iterator PluginSection::begin()
    {
        return mPlugins.begin();
    }

    std::map<std::string, Plugin>::iterator PluginSection::end()
    {
        return mPlugins.end();
    }

    Future<bool> PluginSection::loadFromIni(Threading::Barrier &barrier, const Ini::IniSection &sec)
    {
        std::vector<std::pair<Future<bool>, bool>> futures;

        for (const std::pair<const std::string, std::string> &p : sec) {
            auto it = mPlugins.find(p.first);
            if (it == mPlugins.end()) {
                LOG("Could not find Plugin \"" << p.first << "\"!");
                continue;
            }
            Plugin &plugin = it->second;
            if (p.second.empty()) {
                futures.emplace_back(unloadPlugin(barrier, &plugin), false);
            } else {
                futures.emplace_back(loadPlugin(barrier, &plugin), true);
            }
        }

        std::promise<bool> p;
        Future<bool> f { p.get_future() };

        barrier.queue(nullptr, [futures { std::move(futures) }, promise { std::move(p) }]() mutable {
            bool wait = false;
            for (std::pair<Future<bool>, bool> &p : futures) {
                if (!p.first.isAvailable())
                    wait = true;
                else if (p.first != p.second) {
                    LOG("Error!");
                    promise.set_value(false);
                    return Threading::RETURN;
                }
            }
            if (wait)
                return Threading::YIELD;
            promise.set_value(true);
            return Threading::RETURN;
        });

        return f;
    }

    PluginManager &PluginSection::manager()
    {
        return mMgr;
    }

    const void *PluginSection::getUniqueSymbol(const std::string &name) const
    {
        const void *symbol = nullptr;
        for (const std::pair<const std::string, Plugin> &p : mPlugins) {
            const void *s = p.second.getSymbol(name);
            if (s) {
                if (symbol)
                    std::terminate();
                symbol = s;
            }
        }
        return symbol;
    }

    bool PluginListenerCmp::operator()(PluginListener *first, PluginListener *second) const
    {
        return (first->priority() > second->priority()) || (first->priority() == second->priority() && first < second);
    }

}
}

#endif