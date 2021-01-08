#pragma once

#include "../keyvalueutil/virtualscope.h"

namespace Engine {
namespace Resources {

    struct MODULES_EXPORT ResourceLoaderBase : VirtualScopeBase {

        using ResourceType = ResourceBase;

        ResourceLoaderBase(std::vector<std::string> &&extensions, bool autoLoad = false);
        ResourceLoaderBase(const ResourceLoaderBase &) = delete;
        virtual ~ResourceLoaderBase() = default;

		ResourceLoaderBase &operator=(const ResourceLoaderBase &) = delete;

        virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, const std::string_view &name = {}) = 0;

        template <typename T>
        void resourceAdded(T *res)
        {
            if (mAutoLoad) {
                res->setPersistent(true);
                res->loadData();
            }
        }

        const std::vector<std::string> &fileExtensions() const;

        size_t extensionIndex(const std::string_view &ext) const;

        virtual std::vector<std::pair<std::string_view, TypedScopePtr>> resources() = 0;
        virtual std::vector<const MetaTable *> resourceTypes() const = 0;

    private:
        std::vector<std::string> mExtensions;

        bool mAutoLoad;
    };

}
}


RegisterType(Engine::Resources::ResourceLoaderBase);