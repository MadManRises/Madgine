#pragma once

#include "Interfaces/filesystem/path.h"
#include "Modules/keyvalue/scopebase.h"
#include "Modules/plugins/pluginlistener.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"
#include "resourceloadercollector.h"

namespace Engine {
namespace Resources {
    struct MADGINE_BASE_EXPORT ResourceManager final : ScopeBase
#if ENABLE_PLUGINS
        ,
                                                       Plugins::PluginListener
#endif
    {
        static ResourceManager &getSingleton();

        ResourceManager();
        ~ResourceManager();

        void registerResourceLocation(const Filesystem::Path &path, int priority);

        template <class Loader>
        typename Loader::ResourceType *getResource(const std::string &name)
        {
            return mCollector.get<Loader>().get(name);
        }

        template <class Loader>
        Loader &get()
        {
            return mCollector.get<Loader>();
        }

        template <class Loader>
        std::shared_ptr<typename Loader::Data> load(const std::string &name, bool persistent = false)
        {
            typename Loader::ResourceType *res = getResource<Loader>(name);
            if (res) {
                if (persistent)
                    res->setPersistent(true);
                return res->loadData();
            } else {
                LOG_ERROR("Resource '" << name << "' could not be found!");
                return {};
            }
        }

        void init();

        ResourceLoaderContainer<std::vector> mCollector;

#if ENABLE_PLUGINS
    protected:
        void onPluginLoad(const Plugins::Plugin *plugin) override;
        bool aboutToUnloadPlugin(const Plugins::Plugin *plugin) override;
#endif

    private:
        void updateResources(const Filesystem::Path &path, int priority);
        void updateResources(const Filesystem::Path &path, int priority, const std::map<std::string, ResourceLoaderBase *> &loaderByExtension);

        void updateResource(const Filesystem::Path &path, int priority, const std::map<std::string, ResourceLoaderBase *> &loaderByExtension);

        std::map<std::string, ResourceLoaderBase *> getLoaderByExtension();

    private:
        struct SubDirCompare {
            bool operator()(const Filesystem::Path &first, const Filesystem::Path &second) const
            {
                auto [firstEnd, secondEnd] = std::mismatch(first.str().begin(), first.str().end(), second.str().begin(), second.str().end());
                if (firstEnd == first.str().end() || secondEnd == second.str().end())
                    return false;
                return first.str() < second.str();
            }
        };

        std::map<Filesystem::Path, int, SubDirCompare> mResourcePaths;

        bool mInitialized = false;
    };

}
}