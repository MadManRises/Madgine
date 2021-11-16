#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginsection.h"

#    include "plugin.h"

#    include "pluginmanager.h"

#    include "Interfaces/util/exception.h"

#    include "Interfaces/dl/runtime.h"

#    include "Generic/keyvalue.h"

#    include "ini/inisection.h"

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
        for (auto result : Dl::listSharedLibraries()) {
            auto file = result.path().filename();
            if (std::regex_match(file.str(), match, e)) {
                std::string project = match[1];
                std::string name = match[2];
                auto pib = mPlugins.try_emplace(name, name, this, project, result.path());
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

    Threading::TaskFuture<bool> PluginSection::load(Threading::Barrier &barrier)
    {
        if (mAtleastOne) {
            if (mPlugins.empty())
                throw exception("No plugin available in Section tagged as atleastOne: "s + mName);
            for (std::pair<const std::string, Plugin> &p : mPlugins) {
                Threading::TaskFuture<bool> state = p.second.state(Plugin::LOADING);
                if (!state.is_ready() || state)
                    return state;
            }
            /*if (!*/ return loadPlugin(barrier, &mPlugins.begin()->second); /*)
                throw exception("Failed to load default Plugin for atleastOne Section: "s + mPlugins.begin()->first);*/
        }
        return true;
    }

    Threading::TaskFuture<bool> PluginSection::unload(Threading::Barrier &barrier)
    {
        std::vector<Threading::TaskFuture<bool>> dependentsList;

        for (Plugin *p : mDependents) {
            dependentsList.emplace_back(p->section()->unloadPlugin(barrier, p));
        }

        return barrier.queue(
            nullptr, [](PluginSection *_this, Threading::Barrier &barrier, std::vector<Threading::TaskFuture<bool>> dependentsList) mutable -> Threading::Task<bool> {
                for (Threading::TaskFuture<bool> &f : dependentsList) {
                    if (co_await f) {
                        LOG_ERROR("Unload of dependent for plugin-section \"" << _this->mName << "\" failed!");
                        co_return true;
                    }
                }
                std::vector<Threading::TaskFuture<bool>> results;
                for (Plugin &p : kvValues(_this->mPlugins)) {
                    results.emplace_back(_this->unloadPlugin(barrier, &p));
                }
                for (Threading::TaskFuture<bool> &f : results) {
                    if (co_await f) {
                        LOG_ERROR("Unload of plugin in section \"" << _this->mName << "\" failed!");
                        co_return true;
                    }
                }
                co_return false;
            },
            this, barrier, std::move(dependentsList));
    }

    PluginSection::State PluginSection::isLoaded(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it != mPlugins.end()) {
            Threading::TaskFuture<bool> state = it->second.state();
            if (!state.is_ready())
                return UNDEFINED;
            return state ? LOADED : UNLOADED;
        }
        return UNLOADED;
    }

    Threading::TaskFuture<bool> PluginSection::loadPlugin(const std::string &name, std::function<void()> onSuccess)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        return barrier.queue(
            nullptr, [](Threading::Barrier &barrier, PluginSection *_this, Plugin *plugin, std::function<void()> onSuccess) mutable -> Threading::Task<bool> {
                bool result = co_await _this->loadPlugin(barrier, plugin);
                if (result) {
                    if (onSuccess)
                        onSuccess();
                    _this->mMgr.onUpdate(barrier);
                }
                co_return result;
            },
            barrier, this, plugin, std::move(onSuccess));
    }

    Threading::TaskFuture<bool> PluginSection::unloadPlugin(const std::string &name, std::function<void()> onSuccess)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        Threading::Barrier &barrier = Threading::WorkGroup::barrier(Threading::Barrier::RUN_ONLY_ON_MAIN_THREAD);
        return barrier.queue(
            nullptr, [](Threading::Barrier &barrier, PluginSection *_this, Plugin *plugin, std::function<void()> onSuccess) mutable -> Threading::Task<bool> {
                bool result = co_await _this->unloadPlugin(barrier, plugin);
                if (!result) {
                    if (onSuccess)
                        onSuccess();
                    _this->mMgr.onUpdate(barrier);
                }
                co_return result;
            },
            barrier, this, plugin, std::move(onSuccess));
    }

    Threading::TaskFuture<bool> PluginSection::loadPluginByFilename(const std::string &name)
    {
        auto pib = mPlugins.try_emplace(name, name);
        assert(pib.second);
        return loadPlugin(name);
    }

    Plugin *PluginSection::getPlugin(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it == mPlugins.end())
            return nullptr;
        return &it->second;
    }

    Threading::TaskFuture<bool> PluginSection::loadPlugin(Threading::Barrier &barrier, Plugin *p)
    {
        assert(p->section() == this);

        return p->startOperation(Plugin::LOADING, [=, &barrier]() {
            return barrier.queue(
                nullptr, [](PluginSection *_this, Threading::Barrier &barrier, Plugin *p) -> Threading::Task<bool> {

                    Plugin *unloadExclusive
                        = nullptr;
                    if (_this->mExclusive) {
                        for (Plugin &p2 : kvValues(_this->mPlugins)) {
                            if (&p2 != p && p2.isLoaded()) {
                                assert(!unloadExclusive);
                                unloadExclusive = &p2;
                            }
                        }
                    }

                    if (unloadExclusive) {
                        if (co_await _this->unloadPlugin(barrier, unloadExclusive)) {
                            co_return false;
                        }
                    }

                    if (!co_await p->load(_this->mMgr, barrier)) {
                        co_return false;
                    }

                    PluginSection &toolsSection = _this->mMgr.section("Tools");
                    if (Plugin *toolPlugin = toolsSection.getPlugin(p->name() + "Tools")) {
                        co_return co_await toolsSection.loadPlugin(barrier, toolPlugin);
                    } else {
                        co_return true;
                    }
                },
                this, barrier, p);
        });
    }

    Threading::TaskFuture<bool> PluginSection::unloadPlugin(Threading::Barrier &barrier, Plugin *p)
    {
        assert(p->section() == this);

        return p->startOperation(Plugin::UNLOADING, [=, &barrier]() {
            return p->unload(mMgr, barrier);
        });
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

    Threading::TaskFuture<bool> PluginSection::loadFromIni(Threading::Barrier &barrier, const Ini::IniSection &sec)
    {
        return barrier.queue(
            nullptr, [](PluginSection *_this, Threading::Barrier &barrier, const Ini::IniSection &sec) -> Threading::Task<bool> {
                std::vector<std::pair<Threading::TaskFuture<bool>, bool>> futures;

                for (const std::pair<const std::string, std::string> &p : sec) {
                    Plugin *plugin = _this->getPlugin(p.first);
                    if (!plugin) {
                        LOG("Could not find Plugin \"" << p.first << "\"!");
                        continue;
                    }
                    if (p.second.empty()) {
                        futures.emplace_back(_this->unloadPlugin(barrier, plugin), false);
                    } else {
                        futures.emplace_back(_this->loadPlugin(barrier, plugin), true);
                    }
                }

                for (std::pair<Threading::TaskFuture<bool>, bool> &p : futures) {
                    if (co_await p.first != p.second) {
                        LOG("Error!");
                        co_return false;
                    }
                }
                co_return true;
            },
            this, barrier, sec);
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

}
}

#endif