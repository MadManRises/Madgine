#include "../resourceslib.h"

#include "resourceloaderbase.h"

#include "resourcemanager.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Resources::ResourceLoaderBase)
METATABLE_END(Engine::Resources::ResourceLoaderBase)

namespace Engine {
namespace Resources {

    ResourceLoaderBase::ResourceLoaderBase(std::vector<std::string> &&extensions, const ResourceLoaderSettings &settings)
        : mExtensions(std::forward<std::vector<std::string>>(extensions))
        , mSettings(settings)
    {
    }

    const std::vector<std::string> &ResourceLoaderBase::fileExtensions() const
    {
        return mExtensions;
    }

    size_t ResourceLoaderBase::extensionIndex(std::string_view ext) const
    {
        return std::ranges::find(mExtensions, ext) - mExtensions.begin();
    }

    std::vector<const MetaTable *> ResourceLoaderBase::resourceTypes() const
    {
        return {};
    }

    Threading::TaskQueue *ResourceLoaderBase::loadingTaskQueue() const
    {
        return ResourceManager::getSingleton().taskQueue();
    }

    Threading::TaskFuture<bool> ResourceLoaderBase::queueLoading(Threading::Task<bool> task)
    {
        Threading::TaskFuture<bool> fut = task.get_future();
        loadingTaskQueue()->queueTask(std::move(task));
        return fut;
    }

    Threading::TaskFuture<void> ResourceLoaderBase::queueUnloading(Threading::Task<void> task)
    {
        Threading::TaskFuture<void> fut = task.get_future();
        loadingTaskQueue()->queueTask(std::move(task));
        return fut;
    }

}
}
