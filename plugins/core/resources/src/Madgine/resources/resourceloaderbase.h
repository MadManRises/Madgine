#pragma once

#include "Meta/keyvalue/virtualscope.h"
#include "Modules/threading/task.h"

namespace Engine {
namespace Resources {

    struct ResourceLoaderSettings {
        bool mAutoLoad = false;
        bool mAutoReload = true;
        bool mInplaceReload = false;
    };

    struct MADGINE_RESOURCES_EXPORT ResourceLoaderBase : VirtualScopeBase<> {

        using Resource = ResourceBase;

        ResourceLoaderBase(std::vector<std::string> &&extensions, const ResourceLoaderSettings &settings = {});
        ResourceLoaderBase(const ResourceLoaderBase &) = delete;
        virtual ~ResourceLoaderBase() = default;

        ResourceLoaderBase &operator=(const ResourceLoaderBase &) = delete;

        virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, std::string_view name = {}) = 0;
        virtual void updateResourceData(ResourceBase *res) = 0;

        template <typename T>
        void resourceAdded(T *res)
        {
            if (mSettings.mAutoLoad) {
                res->loadData().info()->setPersistent(true);
            }
        }

        const std::vector<std::string> &fileExtensions() const;

        size_t extensionIndex(std::string_view ext) const;

        virtual std::vector<std::pair<std::string_view, TypedScopePtr>> typedResources() = 0;
        virtual std::vector<const MetaTable *> resourceTypes() const = 0;
        virtual std::vector<ResourceBase *> resources() = 0; 

        virtual Threading::TaskQueue *loadingTaskQueue() const;

    protected:
        std::vector<std::string> mExtensions;

        ResourceLoaderSettings mSettings;
    };

}
}

REGISTER_TYPE(Engine::Resources::ResourceLoaderBase)