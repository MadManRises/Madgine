#include "../moduleslib.h"

#if ENABLE_PLUGINS

#include "plugin.h"

#include "../generic/templates.h"
#include "Interfaces/dl/dlapi.h"

#include "binaryinfo.h"

#include "pluginsection.h"
#include "pluginmanager.h"

#include "../threading/defaulttaskqueue.h"

namespace Engine {
namespace Plugins {

    Plugin::Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path)
        : mModule(nullptr)
        , mProject(std::move(project))
        , mSection(section)
        , mName(std::move(name))
        , mPath(std::move(path))
        , mState(UNLOADED)
    {
        if (mPath.empty() && !mName.empty()) {
#if WINDOWS
            mPath = mName;
#elif UNIX
            mPath = "lib" + mName + ".so";
#endif
        }
    }

    Plugin::~Plugin()
    {
        assert(!mModule);
        assert(mDependencies.empty());
        assert(mDependents.empty());
    }

    Plugin::LoadState Plugin::isLoaded() const
    {
        return mState;
    }

    Plugin::LoadState Plugin::load()
    {
        if (mState != UNLOADED)
            return mState;

        LOG("Loading Plugin \"" << mName << "\"...");

        std::string errorMsg;

        try {
            mModule = openDll(mPath.str());
            if (mModule) {
                const BinaryInfo *bin = info();
                bin->mSelf = this;

                for (const char **dep = bin->mPluginDependencies; *dep; ++dep) {
                    const char *dependencyName = *dep;
                    addDependency(mSection->manager().getPlugin(dependencyName));
                }

                mState = LOADED;
            } else {
                errorMsg = "Unkown";
            }
        } catch (const std::exception &e) {
            errorMsg = e.what();
            mModule = nullptr;
            mState = UNLOADED;
        }

        if (mState == UNLOADED) {
            LOG_ERROR("Load of plugin \"" << mName << "\" failed with error: " << errorMsg);
        } else {

            for (Plugin *dep : mDependencies) {
                LoadState result = dep->mSection->loadPlugin(dep);
                if (result == UNLOADED) {
                    mState = UNLOADED;
                    break;
                } else if (result == DELAYED) {
                    mState = DELAYED;
                }
            }

            auto task = [this]() {
                if (mState == LOADED) {
                    LOG("Success");
                } else if (mState == UNLOADED) {
                    LOG_ERROR("Load of dependency for plugin \"" << mName << "\" failed!");
                    clearDependencies();
                    closeDll(mModule);
                    mModule = nullptr;
                }
            };
            if (mState != DELAYED) {
                task();
            } else {
                Threading::DefaultTaskQueue::getSingleton().queue(std::move(task));
            }
        }

        return mState;
    }

    Plugin::LoadState Plugin::unload()
    {
        if (mState != LOADED)
            return mState;

        LOG("Unloading Plugin \"" << mName << "\".");

        for (Plugin *dep : mDependents) {
            LoadState result = dep->mSection->unloadPlugin(dep);
            if (result == LOADED) {
                mState = LOADED;
                return LOADED;
            } else if (result == DELAYED) {
                mState = DELAYED;
            }
        }

        auto task = [=]() {
            clearDependencies();
            LOG("Unloading Plugin \"" << mName << "\"...");
            closeDll(mModule);

            mModule = nullptr;
            mState = UNLOADED;
        };

        if (mState != DELAYED) {
            task();
        } else {
            Threading::DefaultTaskQueue::getSingleton().queue(std::move(task));
        }

        return mState;        
    }

    const void *Plugin::getSymbol(const std::string &name) const
    {
        std::string fullName = name + "_" + mName;
        return getDllSymbol(mModule, fullName);
    }

    Filesystem::Path Plugin::fullPath() const
    {
        if (mState != LOADED)
            return {};

        std::string fullName = "binaryInfo_" + mName;
        return getDllFullPath(mModule, fullName);
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

    void Plugin::addDependency(Plugin *dependency)
    {
        mDependencies.push_back(dependency);
        dependency->mDependents.push_back(this);
    }

    void Plugin::removeDependency(Plugin *dependency)
    {
        auto it = std::find(mDependencies.begin(), mDependencies.end(), dependency);
        assert(it != mDependencies.end());

        auto it2 = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
        assert(it2 != dependency->mDependents.end());

        mDependencies.erase(it);
        dependency->mDependents.erase(it);
    }

    void Plugin::clearDependencies()
    {
        while (!mDependencies.empty()) {
            Plugin *dependency = mDependencies.back();

            auto it = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
            assert(it != dependency->mDependents.end());

            mDependencies.pop_back();
            dependency->mDependents.erase(it);
        }
    }
}
}

#endif