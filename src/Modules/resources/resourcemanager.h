#pragma once

#include "../keyvalue/scopebase.h"
#include "../plugins/pluginlistener.h"
#include "resourceloadercollector.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

namespace Engine {
namespace Resources {
    struct MODULES_EXPORT ResourceManager final : ScopeBase
#if ENABLE_PLUGINS
        ,
                                                  Plugins::PluginListener
#endif
    {
        static ResourceManager &getSingleton();

        ResourceManager();
        ~ResourceManager();

        void registerResourceLocation(const Filesystem::Path &path, int priority);

        template <typename Loader>
        typename Loader::ResourceType *getResource(const std::string &name)
        {
            return mCollector.get<Loader>().get(name);
        }

        template <typename Loader>
        Loader &get()
        {
            return mCollector.get<Loader>();
        }

        ResourceLoaderBase &get(size_t i)
        {
            return mCollector.get(i);
        }

        void init();

        ResourceLoaderContainer<std::vector<Placeholder<0>>> mCollector;

        Filesystem::Path findResourceFile(const std::string &fileName);

#if ENABLE_PLUGINS
        int priority() const override;
#endif

#if ENABLE_PLUGINS
    protected:
        void onPluginLoad(const Plugins::Plugin *plugin) override;
        Future<void> aboutToUnloadPlugin(const Plugins::Plugin *plugin) override;
#endif

    private:
        void updateResources(const Filesystem::Path &path, int priority);
        void updateResources(const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>> &loaderByExtension);

        void updateResource(const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>> &loaderByExtension);

        std::map<std::string, std::vector<ResourceLoaderBase *>> getLoaderByExtension();

    private:
        struct SubDirCompare {
            bool operator()(const Filesystem::Path &first, const Filesystem::Path &second) const;
        };

        std::map<Filesystem::Path, int, SubDirCompare> mResourcePaths;

        bool mInitialized = false;
    };

}
}