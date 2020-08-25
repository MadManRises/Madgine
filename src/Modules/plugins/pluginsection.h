#pragma once

#if ENABLE_PLUGINS

#    include "../generic/future.h"
#    include "loadstate.h"

namespace Engine {
namespace Plugins {

    struct PluginListenerCmp {
        bool operator()(PluginListener *first, PluginListener *second) const;
    };

    struct MODULES_EXPORT PluginSection {
        PluginSection(PluginManager &mgr, const std::string &name, bool exclusive = false, bool atleastOne = false);
        PluginSection(const PluginSection &) = delete;
        ~PluginSection();
        
        bool isAtleastOne() const;
        bool isExclusive() const;

        Future<bool> load(Threading::Barrier &barrier);
        Future<bool> unload(Threading::Barrier &barrier);

        enum State {
            LOADED,
            UNLOADED,
            UNDEFINED
        };

        State isLoaded(const std::string &name);
        Future<bool> loadPlugin(const std::string &name);
        Future<bool> unloadPlugin(const std::string &name);

        bool loadPluginByFilename(const std::string &name);

        void addListener(PluginListener *listener);
        void removeListener(PluginListener *listener);

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

        Future<bool> loadFromIni(Threading::Barrier &barrier, const Ini::IniSection &sec);

        PluginManager &manager();

        Plugin *getPlugin(const std::string &name);

    private:
        Future<bool> loadPlugin(Threading::Barrier &barrier, Plugin *p, std::optional<std::promise<bool>> &&promise = {}, std::optional<Future<bool>> &&future = {});
        Future<bool> unloadPlugin(Threading::Barrier &barrier, Plugin *p, std::optional<std::promise<bool>> &&promise = {}, std::optional<Future<bool>> &&future = {});

        friend struct Plugin;

        std::map<std::string, Plugin> mPlugins;
        std::set<PluginListener *, PluginListenerCmp> mListeners;

        std::vector<Plugin *> mDependents;

        std::string mName;

        PluginManager &mMgr;

        const bool mAtleastOne;
        const bool mExclusive;

    };

}
}

#endif