#pragma once

#if ENABLE_PLUGINS

#include "loadstate.h"

#include "Interfaces/filesystem/path.h"

#include "../generic/future.h"

namespace Engine {
namespace Plugins {
    struct MODULES_EXPORT Plugin {
        Plugin(std::string name, PluginSection *section = nullptr, std::string project = {});
        Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path);
        ~Plugin();

        void load(PluginManager &manager, Threading::Barrier &barrier, std::promise<bool> &&promise);
        void unload(PluginManager &manager, Threading::Barrier &barrier, std::promise<bool> &&promise);

        const void *getSymbol(const std::string &name) const;

        Filesystem::Path fullPath() const;

        const std::string &project() const;

        const BinaryInfo *info() const;

        bool isDependencyOf(Plugin *p) const;

		const std::string &name() const;

        PluginSection *section() const;

        enum Operation {
            LOADING,
            UNLOADING
        };

        Future<bool> startOperation(Operation op, std::optional<std::promise<bool>> &promise, std::optional<Future<bool>> &&future);

        Future<bool> state();
        Future<bool> state(Operation op);
        bool isLoaded();

        friend struct PluginManager;

    protected:
        void addDependency(PluginManager &manager, Plugin *dependency);
        void removeDependency(PluginManager &manager, Plugin *dependency);
        void addGroupDependency(PluginManager &manager, PluginSection *dependency);
        void removeGroupDependency(PluginManager &manager, PluginSection *dependency);
        void clearDependencies(PluginManager &manager);

        void checkCircularDependency(Plugin *dependency);
        bool checkCircularDependency(Plugin *dependency, std::vector<std::string_view> &trace);

    private:
        void *mModule;

        std::string mProject;
        PluginSection *mSection;
        std::string mName;
        Filesystem::Path mPath;
        
        Future<bool> mState;

        std::vector<Plugin *> mDependencies;
        std::vector<Plugin *> mDependents;
        std::vector<PluginSection *> mGroupDependencies;

        Operation mOperation;

        std::mutex mMutex;
    };
}
}

#endif