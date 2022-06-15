#pragma once

#if ENABLE_PLUGINS

#include "Interfaces/filesystem/path.h"

#include "../threading/task.h"

#include "Interfaces/dl/dlapi.h"

namespace Engine {
namespace Plugins {
    struct MODULES_EXPORT Plugin {
        Plugin(std::string name, PluginSection *section = nullptr, std::string project = {});
        Plugin(std::string name, PluginSection *section, std::string project, Filesystem::Path path);
        ~Plugin();

        const void *getSymbol(const std::string &name) const;

        Filesystem::Path fullPath() const;

        const std::string &project() const;

        const BinaryInfo *info() const;

        bool isDependencyOf(Plugin *p) const;

		const std::string &name() const;

        PluginSection *section() const;


        void setLoaded(bool loaded);
        bool isLoaded() const;

        void loadDependencies(PluginManager &manager);
        void unloadDependents(PluginManager &manager);

        friend struct PluginManager;

    protected:
        void addDependency(PluginManager &manager, Plugin *dependency);
        void removeDependency(PluginManager &manager, Plugin *dependency);
        void addGroupDependency(PluginManager &manager, PluginSection *dependency);
        void removeGroupDependency(PluginManager &manager, PluginSection *dependency);
        void clearDependencies(PluginManager &manager);

        void checkCircularDependency(Plugin *dependency);
        bool checkCircularDependency(Plugin *dependency, std::vector<std::string_view> &trace);

        void ensureModule(PluginManager &manager);

    private:
        Dl::DlHandle mModule;
        bool mIsLoaded = false;

        std::string mProject;
        PluginSection *mSection;
        std::string mName;
        Filesystem::Path mPath;

        std::vector<Plugin *> mDependencies;
        std::vector<Plugin *> mDependents;
        std::vector<PluginSection *> mGroupDependencies;
    };
}
}

#endif