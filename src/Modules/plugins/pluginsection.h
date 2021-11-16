#pragma once

#if ENABLE_PLUGINS

#include "../threading/taskfuture.h"

namespace Engine {
namespace Plugins {

    struct MODULES_EXPORT PluginSection {
        PluginSection(PluginManager &mgr, const std::string &name, bool exclusive = false, bool atleastOne = false);
        PluginSection(const PluginSection &) = delete;
        ~PluginSection();
        
        bool isAtleastOne() const;
        bool isExclusive() const;

        bool load();
        bool unload();

        bool isLoaded(const std::string &name);
        bool loadPlugin(const std::string &name);
        bool unloadPlugin(const std::string &name);

        bool loadPluginByFilename(const std::string &name);

        void loadAllDependencies();

        template <typename T>
        T *getUniqueSymbol(const std::string &name) const
        {
            return static_cast<T *>(getUniqueSymbol(name));
        }
        const void *getUniqueSymbol(const std::string &name) const;

        std::map<std::string, Plugin>::const_iterator begin() const;
        std::map<std::string, Plugin>::const_iterator end() const;
        std::map<std::string, Plugin>::iterator begin();
        std::map<std::string, Plugin>::iterator end();

        PluginManager &manager();

        Plugin *getPlugin(const std::string &name);

    private:
        bool loadPlugin(Plugin *p, bool autoLoadTools = true);
        bool unloadPlugin(Plugin *p);

        friend struct Plugin;

        std::map<std::string, Plugin> mPlugins;

        std::vector<Plugin *> mDependents;

        std::string mName;

        PluginManager &mMgr;

        const bool mAtleastOne;
        const bool mExclusive;

    };

}
}

#endif