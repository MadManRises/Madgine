#pragma once

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"
#include "Interfaces/filesystem/filewatcher.h"
#include "resourceloadercollector.h"
#include "Modules/threading/taskqueue.h"
#include "Generic/systemvariable.h"

#include "Modules/threading/madgineobject.h"

namespace Engine {
namespace Resources {
    struct MADGINE_RESOURCES_EXPORT ResourceManager : Threading::MadgineObject<ResourceManager>
    {
        static ResourceManager &getSingleton();

        ResourceManager(bool toolMode = false);
        ~ResourceManager();

        void registerResourceLocation(const Filesystem::Path &path, int priority);

        template <typename Loader>
        typename Loader::Resource *getResource(const std::string &name)
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

        bool init();
        void finalize();

        ResourceLoaderContainer<std::vector<Placeholder<0>>> mCollector;

        Filesystem::Path findResourceFile(const std::string &fileName);

        void update();

        void waitForInit();

        Threading::TaskQueue *taskQueue();

    private:
        void updateResources(Filesystem::FileEventType event, const Filesystem::Path &path, int priority);
        void updateResources(Filesystem::FileEventType event, const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> &loaderByExtension);

        void updateResource(Filesystem::FileEventType event, const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> &loaderByExtension);

        std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> getLoaderByExtension();

    private:
        struct SubDirCompare {
            bool operator()(const Filesystem::Path &first, const Filesystem::Path &second) const;
        };

        Filesystem::FileWatcher mFileWatcher;

        std::map<Filesystem::Path, int, SubDirCompare> mResourcePaths;

        SystemVariable<bool, false> mInitialized;

        Threading::TaskQueue mIOQueue;
    };

}
}