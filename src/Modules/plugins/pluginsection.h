#pragma once

#if ENABLE_PLUGINS

#    include "loadstate.h"

namespace Engine {
namespace Plugins {

    struct PluginListenerCmp {
        bool operator()(PluginListener *first, PluginListener *second) const;
    };

    struct MODULES_EXPORT PluginSection {
        PluginSection(PluginManager &mgr, const std::string &name);

        void setAtleastOne(bool atleastOne = true);
        bool isAtleastOne() const;
        void setExclusive(bool exclusive = true);
        bool isExclusive() const;

        bool isLoaded(const std::string &name) const;
        LoadState loadPlugin(const std::string &name);
        LoadState unloadPlugin(const std::string &name);

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

        void loadFromIni(Ini::IniSection &sec);

        PluginManager &manager();

        Plugin *getPlugin(const std::string &name);

    private:
        LoadState loadPlugin(Plugin *p);
        LoadState unloadPlugin(Plugin *p);

        friend struct Plugin;

        std::map<std::string, Plugin> mPlugins;
        std::set<PluginListener *, PluginListenerCmp> mListeners;

        bool mAtleastOne = false;
        bool mExclusive = false;

        std::string mName;

        PluginManager &mMgr;
    };

}
}

#endif