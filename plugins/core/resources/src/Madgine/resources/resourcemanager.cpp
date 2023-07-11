#include "../resourceslib.h"

#include "resourcemanager.h"

#include "resourcebase.h"

#include "Modules/plugins/plugin.h"
#include "Modules/plugins/pluginmanager.h"

#include "Interfaces/filesystem/fsapi.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "resourceloaderbase.h"

#include "Modules/plugins/pluginsection.h"

#include "Madgine/root/root.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

#include "Madgine/cli/parameter.h"

UNIQUECOMPONENT(Engine::Resources::ResourceManager)

METATABLE_BEGIN(Engine::Resources::ResourceManager)
MEMBER(mCollector)
METATABLE_END(Engine::Resources::ResourceManager)

namespace Engine {
namespace Resources {

    CLI::Parameter<std::string> exportResources { { "--export-resources", "-er" }, "", "If set the resource manager will write all available resources to the specified list file." };

    static ResourceManager *sSingleton = nullptr;

    ResourceManager &ResourceManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    ResourceManager::ResourceManager(Root::Root &root)
        : RootComponent(root)
    {
        assert(!sSingleton);
        sSingleton = this;

        if (!root.toolMode()) {
            root.taskQueue()->addSetupSteps(
                [this]() { return callInit(); },
                [this]() { return callFinalize(); });
        } else if (!exportResources->empty()) {
            enumerateResources();

            std::map<Filesystem::Path, std::vector<ResourceBase *>> resourceList = buildResourceList();
            std::ofstream out { *exportResources };
            if (!out) {
                LOG_ERROR("Error opening for writing: " << *exportResources);
                mErrorCode = -1;
            }
            for (const auto &[path, resources] : resourceList) {
                out << path << "\n";
            }
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

            if (mEnumerated) {
                updateResources(Filesystem::FileEventType::FILE_CREATED, path, priority);
            }
        }
    }

    void ResourceManager::enumerateResources()
    {
        assert(!mEnumerated);

#if ENABLE_PLUGINS
        Plugins::PluginManager &pMgr = Plugins::PluginManager::getSingleton();
        for (auto &section : pMgr) {
            for (Plugins::Plugin &p : section) {
                if (!p.isLoaded(pMgr.selection()))
                    continue;
                const Plugins::BinaryInfo *info = p.info();
                Filesystem::Path binPath = info->mBinaryDir;
                bool isLocal = p.fullPath().parentPath() == binPath;
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

        mEnumerated = true;
    }

    bool ResourceManager::init()
    {
        enumerateResources();

        taskQueue()->queueTask(update());

        return true;
    }

    void ResourceManager::finalize()
    {
        mFileWatcher.clear();
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

    Threading::Task<void> ResourceManager::update()
    {
        while (taskQueue()->running()) {
            std::vector<Filesystem::FileEvent> events = mFileWatcher.fetchChangesReduced();

            std::map<std::string, std::vector<ResourceLoaderBase *>, std::less<>> loaderByExtension = getLoaderByExtension();

            for (const Filesystem::FileEvent &event : events) {
                updateResource(event.mType, event.mPath, mResourcePaths.at(event.mPath), loaderByExtension);
            }

            co_await 1s;
        }
    }

    void ResourceManager::waitForInit()
    {
        mEnumerated.wait();
    }

    Threading::TaskQueue *ResourceManager::taskQueue()
    {
        return mRoot.taskQueue();
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

    std::map<Filesystem::Path, std::vector<ResourceBase *>> ResourceManager::buildResourceList()
    {
        std::map<Filesystem::Path, std::vector<ResourceBase *>> result;
        for (const std::unique_ptr<ResourceLoaderBase> &loader : mCollector) {
            for (ResourceBase *res : loader->resources()) {
                Filesystem::Path path = res->path();
                if (!path.empty() && !path.isRelative(BINARY_DIR)) {
                    if (path.isRelative(SOURCE_DIR))
                        path = path.relative(SOURCE_DIR);
                    result[path].push_back(res);
                }
            }
        }
        return result;
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
