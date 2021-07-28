#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "plugin.h"

#    include "Interfaces/dl/dlapi.h"

#    include "binaryinfo.h"

#    include "pluginmanager.h"
#    include "pluginsection.h"

#    include "../threading/barrier.h"

namespace Engine {
namespace Plugins {

    Plugin::Plugin(std::string name, PluginSection *section, std::string project)
        : mModule(nullptr)
        , mProject(std::move(project))
        , mSection(section)
        , mName(std::move(name))
#    if WINDOWS
        , mPath(mName)
#    elif UNIX
        , mPath("lib" + mName + ".so")
#    endif
        , mState(false)
    {
        assert(!mName.empty());
    }

    Plugin::Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path)
        : mModule(nullptr)
        , mProject(std::move(project))
        , mSection(section)
        , mName(std::move(name))
        , mPath(std::move(path))
        , mState(false)
    {
        assert(!mName.empty());
    }

    Plugin::~Plugin()
    {
        if (mModule && mPath.empty()) {
            Dl::closeDll(mModule);
            mModule = nullptr;
        }
        assert(!mModule);
        assert(mDependencies.empty());
        assert(mDependents.empty());
    }

    void Plugin::load(PluginManager &manager, Threading::Barrier &barrier, std::promise<bool> &&promise)
    {
        LOG("Loading Plugin \"" << mName << "\"...");

        std::string errorMsg;

        try {
            mModule = Dl::openDll(mPath.str());
            if (mModule) {
                const BinaryInfo *bin = info();
                if (bin) {
                    bin->mSelf = this;

                    for (const char **dep = bin->mPluginDependencies; *dep; ++dep) {
                        addDependency(manager, manager.getPlugin(*dep));
                    }

                    for (const char **dep = bin->mPluginGroupDependencies; *dep; ++dep) {
                        addGroupDependency(manager, &manager.section(*dep));
                    }
                } else {
                    errorMsg = "Unable to locate BinaryInfo. Make sure you call generate_binary_info in your CMakeLists.txt for your executable!";
                    Dl::closeDll(mModule);
                    mModule = nullptr;
                }
            } else {
                Dl::DlAPIResult result = Dl::getError("openDll");
                errorMsg = result.toString();
            }
        } catch (const std::exception &e) {
            errorMsg = e.what();
            mModule = nullptr;
        }

        if (!mModule) {
            LOG_ERROR("Load of plugin \"" << mName << "\" failed with error: " << errorMsg);
            promise.set_value(false);
            return;
        } else {

            std::vector<SharedFuture<bool>> dependencyList;
            {
                std::lock_guard lock { mMutex };
                for (Plugin *dep : mDependencies) {
                    if (mName == dep->mName + "Tools") {
                        SharedFuture<bool> f = dep->state();
                        if (f.is_ready() && !f)
                            dependencyList.emplace_back(dep->mSection->loadPlugin(barrier, dep));
                    } else {
                        dependencyList.emplace_back(dep->mSection->loadPlugin(barrier, dep));
                    }
                }
                for (PluginSection *sec : mGroupDependencies) {
                    dependencyList.emplace_back(sec->load(barrier));
                }
            }

            barrier.queue(nullptr, [this, &manager, dependencyList { std::move(dependencyList) }, promise { std::move(promise) }, name { mName }]() mutable {
                bool wait = false;
                for (SharedFuture<bool> &f : dependencyList) {
                    if (!f.is_ready()) {
                        wait = true;
                    } else if (!f) {
                        LOG_ERROR("Load of dependency for plugin \"" << mName << "\" failed!");
                        clearDependencies(manager);
                        Dl::closeDll(mModule);
                        mModule = nullptr;
                        promise.set_value(false);
                        return Threading::RETURN;
                    }
                }
                if (wait) {
                    return Threading::YIELD;
                } else {
                    LOG("Success (" << name << ")");
                    promise.set_value(true);
                    return Threading::RETURN;
                }
            });
        }
    }

    void Plugin::unload(PluginManager &manager, Threading::Barrier &barrier, std::promise<bool> &&promise)
    {
        std::vector<SharedFuture<bool>> dependentList;
        std::vector<Plugin *> dependents = mDependents;
        for (Plugin *dep : dependents) {
            dependentList.emplace_back(dep->mSection->unloadPlugin(barrier, dep));
        }

        barrier.queue(nullptr, [this, &manager, dependentList { std::move(dependentList) }, promise { std::move(promise) }]() mutable {
            bool wait = false;
            for (SharedFuture<bool> &f : dependentList) {
                if (!f.is_ready()) {
                    wait = true;
                } else if (f) {
                    LOG_ERROR("Unload of dependent for plugin \"" << mName << "\" failed!");
                    promise.set_value(true);
                    return Threading::RETURN;
                }
            }
            if (wait) {
                return Threading::YIELD;
            } else {
                clearDependencies(manager);
                LOG("Unloading Plugin \"" << mName << "\"...");
                Dl::closeDll(mModule);

                mModule = nullptr;
                promise.set_value(false);
                return Threading::RETURN;
            }
        });
    }

    const void *Plugin::getSymbol(const std::string &name) const
    {
        std::string fullName = name + "_" + mName;
        return Dl::getDllSymbol(mModule, fullName);
    }

    Filesystem::Path Plugin::fullPath() const
    {

        std::string fullName = "binaryInfo_" + mName;
        return Dl::getDllFullPath(mModule, fullName);
    }

    const std::string &Plugin::project() const
    {
        return mProject;
    }

    const BinaryInfo *Plugin::info() const
    {
        return static_cast<const BinaryInfo *>(getSymbol("binaryInfo"));
    }

    bool Plugin::isDependencyOf(Plugin *p) const
    {
        if (p == this)
            return true;
        for (Plugin *p2 : mDependents) {
            if (p2->isDependencyOf(p))
                return true;
        }
        return false;
    }

    const std::string &Plugin::name() const
    {
        return mName;
    }

    PluginSection *Plugin::section() const
    {
        return mSection;
    }

    SharedFuture<bool> Plugin::startOperation(Operation op, std::optional<std::promise<bool>> &promise, std::optional<SharedFuture<bool>> &&future)
    {
        std::lock_guard lock { mMutex };
        if (mState.is_ready()) {
            if (mState == (op == LOADING)) {
                if (promise) {
                    promise->set_value(mState);
                    promise.reset();
                }
            } else {
                if (!promise)
                    promise.emplace();
                mOperation = op;
                if (future)
                    mState = std::move(*future);
                else
                    mState = promise->get_future().share();
            }
        } else {
            assert(!promise);
            assert(mOperation == op);
        }
        return mState;
    }

    SharedFuture<bool> Plugin::state()
    {
        std::lock_guard lock { mMutex };
        return mState;
    }

    SharedFuture<bool> Plugin::state(Operation op)
    {
        std::lock_guard lock { mMutex };
        assert(mState.is_ready() || mOperation == op);
        return mState;
    }

    bool Plugin::isLoaded()
    {
        return mState.is_ready() && mState;
    }

    void Plugin::addDependency(PluginManager &manager, Plugin *dependency)
    {
        std::lock_guard lock { manager.mDependenciesMutex };
        checkCircularDependency(dependency);
        mDependencies.push_back(dependency);
        dependency->mDependents.push_back(this);
    }

    void Plugin::removeDependency(PluginManager &manager, Plugin *dependency)
    {
        std::lock_guard lock { manager.mDependenciesMutex };
        auto it = std::find(mDependencies.begin(), mDependencies.end(), dependency);
        assert(it != mDependencies.end());

        auto it2 = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
        assert(it2 != dependency->mDependents.end());

        mDependencies.erase(it);
        dependency->mDependents.erase(it2);
    }

    void Plugin::addGroupDependency(PluginManager &manager, PluginSection *dependency)
    {
        std::lock_guard lock { manager.mDependenciesMutex };
        for (std::pair<const std::string, Plugin> &p : *dependency) {
            checkCircularDependency(&p.second);
        }
        mGroupDependencies.push_back(dependency);
        dependency->mDependents.push_back(this);
    }

    void Plugin::removeGroupDependency(PluginManager &manager, PluginSection *dependency)
    {
        std::lock_guard lock { manager.mDependenciesMutex };
        auto it = std::find(mGroupDependencies.begin(), mGroupDependencies.end(), dependency);
        assert(it != mGroupDependencies.end());

        auto it2 = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
        assert(it2 != dependency->mDependents.end());

        mGroupDependencies.erase(it);
        dependency->mDependents.erase(it2);
    }

    void Plugin::clearDependencies(PluginManager &manager)
    {
        std::lock_guard lock { manager.mDependenciesMutex };
        while (!mDependencies.empty()) {
            Plugin *dependency = mDependencies.back();

            auto it = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
            assert(it != dependency->mDependents.end());

            mDependencies.pop_back();
            dependency->mDependents.erase(it);
        }

        while (!mGroupDependencies.empty()) {
            PluginSection *dependency = mGroupDependencies.back();

            auto it = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
            assert(it != dependency->mDependents.end());

            mGroupDependencies.pop_back();
            dependency->mDependents.erase(it);
        }
    }

    void Plugin::checkCircularDependency(Plugin *dependency)
    {
        std::vector<std::string_view> trace { mName };
        if (checkCircularDependency(dependency, trace)) {
            LOG_ERROR("Detected Circular Plugin-Dependency: " << StringUtil::join(trace, " -> "));
            throw 0;
        }
    }

    bool Plugin::checkCircularDependency(Plugin *dependency, std::vector<std::string_view> &trace)
    {
        trace.push_back(dependency->mName);
        if (dependency == this)
            return true;

        for (Plugin *p : dependency->mDependencies) {
            if (checkCircularDependency(p, trace))
                return true;
        }

        for (PluginSection *sec : dependency->mGroupDependencies) {
            trace.push_back(sec->mName);
            for (std::pair<const std::string, Plugin> &p : *sec) {
                if (checkCircularDependency(&p.second, trace))
                    return true;
            }
            trace.pop_back();
        }
        trace.pop_back();

        return false;
    }
}
}

#endif