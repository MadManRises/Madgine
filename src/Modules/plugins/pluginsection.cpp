#include "../moduleslib.h"

#if ENABLE_PLUGINS

#    include "pluginsection.h"

#    include "plugin.h"

#    include "pluginmanager.h"

#    include "Interfaces/util/exception.h"

#    include "Interfaces/dl/runtime.h"

#    include "Generic/keyvalue.h"

#    include "ini/inisection.h"

#    include "../threading/workgroup.h"

namespace Engine {
namespace Plugins {

    PluginSection::PluginSection(PluginManager &mgr, const std::string &name, bool exclusive, bool atleastOne)
        : mName(name)
        , mMgr(mgr)
        , mAtleastOne(atleastOne)
        , mExclusive(exclusive)
    {
        const std::regex e { SHARED_LIB_PREFIX "Plugin_([a-zA-Z0-9]*)_" + mName + "_([a-zA-Z0-9]*)\\" SHARED_LIB_SUFFIX };
        std::smatch match;
        for (auto result : Dl::listSharedLibraries()) {
            auto file = result.path().filename();
            if (std::regex_match(file.str(), match, e)) {
                std::string project = match[1];
                std::string name = match[2];
                auto pib = mPlugins.try_emplace(name, name, this, project, result.path());
                assert(pib.second);
            }
        }
    }

    PluginSection::~PluginSection()
    {
        assert(mDependents.empty());
    }

    bool PluginSection::isAtleastOne() const
    {
        return mAtleastOne;
    }

    bool PluginSection::isExclusive() const
    {
        return mExclusive;
    }

    bool PluginSection::load()
    {
        if (mAtleastOne) {
            if (mPlugins.empty())
                throw exception("No plugin available in Section tagged as atleastOne: "s + mName);
            for (Plugin &p : kvValues(mPlugins)) {
                if (p.isLoaded())
                    return true;
            }
            return loadPlugin(&mPlugins.begin()->second);
        }
        return true;
    }

    bool PluginSection::unload()
    {
        for (Plugin *p : mDependents) {
            if (!p->section()->unloadPlugin(p))
                return true;
        }

        for (Plugin &p : kvValues(mPlugins)) {
            if (unloadPlugin(&p))
                return true;
        }

        return false;
    }

    bool PluginSection::isLoaded(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it != mPlugins.end()) {
            return it->second.isLoaded();
        }
        return false;
    }

    bool PluginSection::loadPlugin(const std::string &name)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        return loadPlugin(plugin);
    }

    bool PluginSection::unloadPlugin(const std::string &name)
    {
        Plugin *plugin = getPlugin(name);
        if (!plugin)
            return false;
        return unloadPlugin(plugin);
    }

    bool PluginSection::loadPluginByFilename(const std::string &name)
    {
        auto pib = mPlugins.try_emplace(name, name);
        assert(pib.second);
        return loadPlugin(&pib.first->second);
    }

    Plugin *PluginSection::getPlugin(const std::string &name)
    {
        auto it = mPlugins.find(name);
        if (it == mPlugins.end())
            return nullptr;
        return &it->second;
    }

    bool PluginSection::loadPlugin(Plugin *p, bool autoLoadTools)
    {
        assert(p->section() == this);

        if (mExclusive) {
            Plugin *unloadExclusive = nullptr;

            for (Plugin &p2 : kvValues(mPlugins)) {
                if (&p2 != p && p2.isLoaded()) {
                    assert(!unloadExclusive);
                    unloadExclusive = &p2;
                }
            }

            if (unloadExclusive) {
                if (unloadPlugin(unloadExclusive)) {
                    return false;
                }
            }
        }

        p->setLoaded(true);
        p->loadDependencies(mMgr);

        PluginSection &toolsSection = mMgr.section("Tools");
        Plugin *toolPlugin = toolsSection.getPlugin(p->name() + "Tools");
        if (autoLoadTools && toolPlugin) {
            return toolsSection.loadPlugin(toolPlugin);
        } else {
            mMgr.onUpdate();
            return true;
        }
    }

    bool PluginSection::unloadPlugin(Plugin *p)
    {
        assert(!mAtleastOne);

        p->unloadDependents(mMgr);
        p->setLoaded(false);

        return true;
    }

    std::map<std::string, Plugin>::const_iterator PluginSection::begin() const
    {
        return mPlugins.begin();
    }

    std::map<std::string, Plugin>::const_iterator PluginSection::end() const
    {
        return mPlugins.end();
    }

    std::map<std::string, Plugin>::iterator PluginSection::begin()
    {
        return mPlugins.begin();
    }

    std::map<std::string, Plugin>::iterator PluginSection::end()
    {
        return mPlugins.end();
    }

    void PluginSection::loadAllDependencies()
    {
        for (Plugin& p : kvValues(mPlugins)) {
            if (p.isLoaded())
                p.loadDependencies(mMgr);
        }
    }

    PluginManager &PluginSection::manager()
    {
        return mMgr;
    }

    const void *PluginSection::getUniqueSymbol(const std::string &name) const
    {
        const void *symbol = nullptr;
        for (const std::pair<const std::string, Plugin> &p : mPlugins) {
            const void *s = p.second.getSymbol(name);
            if (s) {
                if (symbol)
                    std::terminate();
                symbol = s;
            }
        }
        return symbol;
    }

}
}

#endif