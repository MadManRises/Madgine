#include "../resourceslib.h"

#include "resourcemanager.h"

#include "resourcebase.h"

#include "Modules/plugins/plugin.h"
#include "Modules/plugins/pluginmanager.h"

#include "Interfaces/filesystem/api.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "resourceloaderbase.h"

#include "Modules/plugins/pluginsection.h"

#include "Madgine/root/root.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Resources::ResourceManager)

namespace Engine {
namespace Resources {

    static ResourceManager *sSingleton = nullptr;

    ResourceManager &ResourceManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    ResourceManager::ResourceManager(Root::Root &root)
        : RootComponent(root)
        , mIOQueue("IO")
    {
        assert(!sSingleton);
        sSingleton = this;

        if (!root.toolMode()) {
            mIOQueue.addSetupSteps(
                [this]() { return callInit(); },
                [this]() { return callFinalize(); });
        }
    }

    ResourceManager::~ResourceManager()
    {
    }

    std::string_view ResourceManager::key() const
    {
        return "ResourceManager";
    }

    void ResourceManager::registerResourceLocation(const Filesystem::Path &path, int priority)
    {
        Filesystem::Path absolutePath = path.absolute();

        if (!exists(absolutePath))
            return;

        auto [it, b] = mResourcePaths.try_emplace(absolutePath, priority);
        if (b) {
            mFileWatcher.addWatch(absolutePath);

            if (mInitialized) {
                updateResources(Filesystem::FileEventType::FILE_CREATED, path, priority);
            }
        }
    }

    bool ResourceManager::init()
    {
#if ENABLE_PLUGINS
        for (auto &section : Plugins::PluginManager::getSingleton()) {
            for (std::pair<const std::string, Plugins::Plugin> &p : section.second) {
                if (!p.second.isLoaded())
                    continue;
                const Plugins::BinaryInfo *info = p.second.info();
                Filesystem::Path binPath = info->mBinaryDir;
                bool isLocal = p.second.fullPath().parentPath() == binPath;
                if (isLocal)
                    registerResourceLocation(Filesystem::Path { info->mProjectRoot } / "data", 75);
                //else
                //registerResourceLocation(binPath.parent_path() / "data" / plugin->());
            }
        }
#endif

        registerResourceLocation(Filesystem::executablePath().parentPath() / "data", 50);

#if ANDROID
        registerResourceLocation("assets:", 25);
#endif

        std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> loaderByExtension = getLoaderByExtension();

        for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths) {
            updateResources(Filesystem::FileEventType::FILE_CREATED, p.first, p.second, loaderByExtension);
        }

        mIOQueue.addRepeatedTask([this]() { update(); }, std::chrono::seconds { 1 });

        mInitialized = true;

        return true;
    }

    void ResourceManager::finalize()
    {
        mFileWatcher.clear();

        mInitialized = false;
    }

    Filesystem::Path ResourceManager::findResourceFile(const std::string &fileName)
    {
        for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths) {
            for (Filesystem::Path p : Filesystem::listFilesRecursive(p.first)) {
                if (p.filename() == fileName)
                    return p;
            }
        }
        return {};
    }

    void ResourceManager::update()
    {
        std::vector<Filesystem::FileEvent> events = mFileWatcher.fetchChangesReduced();

        std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> loaderByExtension = getLoaderByExtension();

        for (const Filesystem::FileEvent &event : events) {
            updateResource(event.mType, event.mPath, mResourcePaths.at(event.mPath), loaderByExtension);
        }
    }

    void ResourceManager::waitForInit()
    {
        mInitialized.wait();
    }

    Threading::TaskQueue *ResourceManager::taskQueue()
    {
        return &mIOQueue;
    }

    void ResourceManager::updateResources(Filesystem::FileEventType event, const Filesystem::Path &path, int priority)
    {
        updateResources(event, path, priority, getLoaderByExtension());
    }

    void ResourceManager::updateResources(Filesystem::FileEventType event, const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> &loaderByExtension)
    {
        LOG_DEBUG("Scanning for Resources in: " << path);

        for (Filesystem::Path p : Filesystem::listFilesRecursive(path)) {
            updateResource(event, p, priority, loaderByExtension);
        }
    }

    void ResourceManager::updateResource(Filesystem::FileEventType event, const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> &loaderByExtension)
    {
        LOG_DEBUG("Updating Resource: " << path << " (" << event << ")");

        std::string extension = StringUtil::toLower(path.extension());

        auto it = loaderByExtension.find(extension);
        if (it != loaderByExtension.end()) {
            for (ResourceLoaderBase *loader : it->second) {
                auto [resource, created] = loader->addResource(path);

                switch (event) {
                case Filesystem::FileEventType::FILE_CREATED:
                    if (!created && path != resource->path()) {
                        int otherPriority = mResourcePaths.at(resource->path());
                        if (priority > otherPriority || (priority == otherPriority && loader->extensionIndex(extension) < loader->extensionIndex(resource->path().extension()))) {
                            resource->setPath(path);
                            loader->updateResourceData(resource);
                        }
                    }
                    break;
                case Filesystem::FileEventType::FILE_MODIFIED:
                    if (!created)
                        loader->updateResourceData(resource);
                    break;
                }
            }
        }
    }

    std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> ResourceManager::getLoaderByExtension()
    {
        std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> loaderByExtension;

        for (const std::unique_ptr<ResourceLoaderBase> &loader : mCollector) {
            for (const std::string &ext : loader->fileExtensions()) {
                loaderByExtension[ext].push_back(loader.get());
            }
        }
        return loaderByExtension;
    }

    bool ResourceManager::SubDirCompare::operator()(const Filesystem::Path &first, const Filesystem::Path &second) const
    {
        auto [firstEnd, secondEnd] = std::mismatch(first.str().begin(), first.str().end(), second.str().begin(), second.str().end());
        if (firstEnd == first.str().end() || secondEnd == second.str().end())
            return false;
        return first < second;
    }

}
}

METATABLE_BEGIN(Engine::Resources::ResourceManager)
MEMBER(mCollector)
METATABLE_END(Engine::Resources::ResourceManager)