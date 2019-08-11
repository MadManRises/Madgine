#pragma once

#if ENABLE_PLUGINS

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Plugins {
    struct MODULES_EXPORT Plugin {
        Plugin(std::string name = {}, PluginSection *section = nullptr, std::string project = {}, Filesystem::Path path = {});
        ~Plugin();

        enum LoadState {
            UNLOADED,
            DELAYED,
            LOADED
        };

        LoadState isLoaded() const;
        LoadState load();
        LoadState unload();

        const void *getSymbol(const std::string &name) const;

        Filesystem::Path fullPath() const;

        const std::string &project() const;

        const BinaryInfo *info() const;

        bool isDependencyOf(Plugin *p) const;

    protected:
        void addDependency(Plugin *dependency);
        void removeDependency(Plugin *dependency);
        void clearDependencies();

    private:
        void *mModule;

        std::string mProject;
        PluginSection *mSection;
        std::string mName;
        Filesystem::Path mPath;
        LoadState mState;

        std::vector<Plugin *> mDependencies;
        std::vector<Plugin *> mDependents;
    };
}
}

#endif