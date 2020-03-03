#include "../moduleslib.h"

#include "resourcemanager.h"

#include "resourcebase.h"

#include "../plugins/plugin.h"
#include "../plugins/pluginmanager.h"

#include "Interfaces/filesystem/api.h"

#include "../keyvalue/metatable_impl.h"

namespace Engine {
namespace Resources {

    static ResourceManager *sSingleton = nullptr;

    ResourceManager &ResourceManager::getSingleton()
    {
        assert(sSingleton);
        return *sSingleton;
    }

    ResourceManager::ResourceManager()
        : mCollector()
    {
        assert(!sSingleton);
        sSingleton = this;

#if ENABLE_PLUGINS
        Plugins::PluginManager::getSingleton().addListener(this);
#endif

        registerResourceLocation(Filesystem::configPath().parentPath() / "data", 50);

#if ANDROID
        registerResourceLocation("assets:", 25);
#endif
    }

    ResourceManager::~ResourceManager()
    {
#if ENABLE_PLUGINS
        Plugins::PluginManager::getSingleton().removeListener(this);
#endif
    }

    void ResourceManager::registerResourceLocation(const Filesystem::Path &path, int priority)
    {

        if (!exists(path))
            return;

        auto [it, b] = mResourcePaths.try_emplace(path, priority);
        /*if (!b)
				std::terminate();*/

        if (mInitialized) {
            updateResources(path, priority);
        }
    }

    void ResourceManager::init()
    {
        mInitialized = true;

        std::map<std::string, std::vector<ResourceLoaderBase *>> loaderByExtension = getLoaderByExtension();

        for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths) {
            updateResources(p.first, p.second, loaderByExtension);
        }
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

#if ENABLE_PLUGINS
    void ResourceManager::onPluginLoad(const Plugins::Plugin *plugin)
    {
        std::map<std::string, std::vector<ResourceLoaderBase *>> loaderByExtension = getLoaderByExtension();

        if (mInitialized) {
            for (const std::pair<const Filesystem::Path, int> &p : mResourcePaths) {
                updateResources(p.first, p.second, loaderByExtension);
            }
        }

        const Plugins::BinaryInfo *info = plugin->info();
        Filesystem::Path binPath = info->mBinaryDir;
        bool isLocal = plugin->fullPath().parentPath() == binPath;
        if (isLocal)
            registerResourceLocation(Filesystem::Path { info->mProjectRoot } / "data", 75);
        //else
        //registerResourceLocation(binPath.parent_path() / "data" / plugin->());
    }

    bool ResourceManager::aboutToUnloadPlugin(const Plugins::Plugin *plugin)
    {
        return false;
    }
#endif

    void ResourceManager::updateResources(const Filesystem::Path &path, int priority)
    {
        updateResources(path, priority, getLoaderByExtension());
    }

    void ResourceManager::updateResources(const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>> &loaderByExtension)
    {
        for (Filesystem::Path p : Filesystem::listFilesRecursive(path)) {
            updateResource(p, priority, loaderByExtension);
        }
    }

    void ResourceManager::updateResource(const Filesystem::Path &path, int priority, const std::map<std::string, std::vector<ResourceLoaderBase *>> &loaderByExtension)
    {
        std::string extension = path.extension();

        auto it = loaderByExtension.find(extension);
        if (it != loaderByExtension.end()) {
            for (ResourceLoaderBase *loader : it->second) {
                auto [resource, b] = loader->addResource(path);

                if (!b) {
                    int otherPriority = mResourcePaths.at(resource->path());
                    if (priority > otherPriority || (priority == otherPriority && loader->extensionIndex(extension) < loader->extensionIndex(resource->path().extension()))) {
                        resource->updatePath(path);
                    }
                }
            }
        }
    }

    std::map<std::string, std::vector<ResourceLoaderBase *>> ResourceManager::getLoaderByExtension()
    {
        std::map<std::string, std::vector<ResourceLoaderBase *>> loaderByExtension;

        for (const std::unique_ptr<ResourceLoaderBase> & loader : mCollector) {
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
        return first.str() < second.str();
    }

#if ENABLE_PLUGINS
    int ResourceManager::priority() const
    {
        return 75;
    }
#endif

}
}

METATABLE_BEGIN(Engine::Resources::ResourceManager)
MEMBER(mCollector)
METATABLE_END(Engine::Resources::ResourceManager)