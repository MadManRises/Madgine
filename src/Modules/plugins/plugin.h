#pragma once

#if ENABLE_PLUGINS

#include "Interfaces/filesystem/path.h"

#include "Modules/threading/task.h"

namespace Engine {
namespace Plugins {
    struct MODULES_EXPORT Plugin {
        Plugin(std::string name, PluginSection *section = nullptr, std::string project = {});
        Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path);
        ~Plugin();

        Threading::TaskFuture<bool> load(PluginManager &manager, Threading::Barrier &barrier);
        Threading::TaskFuture<bool> unload(PluginManager &manager, Threading::Barrier &barrier);

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

        Threading::TaskFuture<bool> startOperation(Operation op, std::function<Threading::TaskFuture<bool>()> task);

        Threading::TaskFuture<bool> state();
        Threading::TaskFuture<bool> state(Operation op);
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
        
        Threading::TaskFuture<bool> mState;

        std::vector<Plugin *> mDependencies;
        std::vector<Plugin *> mDependents;
        std::vector<PluginSection *> mGroupDependencies;

        Operation mOperation;

        std::mutex mMutex;
    };
}
}

#endif