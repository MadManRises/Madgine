#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "plugin.h"

#    include "Interfaces/dl/dlapi.h"

#    include "binaryinfo.h"

#    include "pluginmanager.h"
#    include "pluginsection.h"

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
    {
        assert(!mName.empty());
    }

    Plugin::Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path)
        : mModule(nullptr)
        , mProject(std::move(project))
        , mSection(section)
        , mName(std::move(name))
        , mPath(std::move(path))
    {
        assert(!mName.empty());
    }

    Plugin::~Plugin()
    {
        //assert(!mModule);
        //assert(mDependencies.empty());
        //assert(mDependents.empty());
    }

    void Plugin::ensureModule(PluginManager &manager)
    {
        if (!mModule) {
            std::string errorMsg;
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
                    LOG_ERROR("Unable to locate BinaryInfo. Make sure you call generate_binary_info in your CMakeLists.txt for your binaries!");
                    Dl::closeDll(mModule);
                    mModule = nullptr;
                    std::terminate();                    
                }
            } else {
                Dl::DlAPIResult result = Dl::getError("openDll");
                LOG_ERROR(result.toString());
                std::terminate();
            }
        }
    }

    void Plugin::loadDependencies(PluginManager &manager)
    {
        assert(mIsLoaded);
        ensureModule(manager);
        for (Plugin *dep : mDependencies) {
            if (!dep->isLoaded()) {
                if (mName == dep->mName + "Tools") {
                    dep->mSection->loadPlugin(dep, false);
                } else {
                    dep->mSection->loadPlugin(dep);
                }
            }
        }
        for (PluginSection *sec : mGroupDependencies) {
            sec->load();
        }
    }

    void Plugin::unloadDependents(PluginManager &manager)
    {
        assert(mIsLoaded);
        ensureModule(manager);
        for (Plugin *dep : mDependents) {
            if (dep->mSection->unloadPlugin(dep)) {
                LOG_ERROR("Unload of dependent for plugin \"" << mName << "\" failed!");
                throw 0;
            }
        }
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
        assert(mModule);
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

    bool Plugin::isLoaded() const
    {
        return mIsLoaded;
    }

    void Plugin::setLoaded(bool loaded)
    {
        if (mIsLoaded != loaded) {
            LOG("Plugin " << mName << "... " << (loaded ? "Loaded" : "Unloaded"));
            mIsLoaded = loaded;
        }
    }

    void Plugin::addDependency(PluginManager &manager, Plugin *dependency)
    {
        checkCircularDependency(dependency);
        mDependencies.push_back(dependency);
        dependency->mDependents.push_back(this);
    }

    void Plugin::removeDependency(PluginManager &manager, Plugin *dependency)
    {
        auto it = std::find(mDependencies.begin(), mDependencies.end(), dependency);
        assert(it != mDependencies.end());

        auto it2 = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
        assert(it2 != dependency->mDependents.end());

        mDependencies.erase(it);
        dependency->mDependents.erase(it2);
    }

    void Plugin::addGroupDependency(PluginManager &manager, PluginSection *dependency)
    {
        for (std::pair<const std::string, Plugin> &p : *dependency) {
            checkCircularDependency(&p.second);
        }
        mGroupDependencies.push_back(dependency);
        dependency->mDependents.push_back(this);
    }

    void Plugin::removeGroupDependency(PluginManager &manager, PluginSection *dependency)
    {
        auto it = std::find(mGroupDependencies.begin(), mGroupDependencies.end(), dependency);
        assert(it != mGroupDependencies.end());

        auto it2 = std::find(dependency->mDependents.begin(), dependency->mDependents.end(), this);
        assert(it2 != dependency->mDependents.end());

        mGroupDependencies.erase(it);
        dependency->mDependents.erase(it2);
    }

    void Plugin::clearDependencies(PluginManager &manager)
    {
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

        assert(mModule);
        Dl::closeDll(mModule);
        mModule = nullptr;
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