#pragma once

#include "../keyvalue/scopebase.h"

namespace Engine {
namespace Resources {

    struct MODULES_EXPORT ResourceLoaderBase : ScopeBase {
        ResourceLoaderBase(std::vector<std::string> &&extensions, bool autoLoad = false);
        virtual ~ResourceLoaderBase() = default;

        virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, const std::string &name = {}) = 0;

        template <typename T>
        void resourceAdded(T *res)
        {
            if (mAutoLoad) {
                res->setPersistent(true);
                res->load();
            }
        }

        const std::vector<std::string> &fileExtensions() const;

        size_t extensionIndex(const std::string &ext) const;

		virtual std::vector<std::pair<std::string, TypedScopePtr>> resources() = 0;
        virtual const MetaTable *resourceType() const = 0;

    private:
        std::vector<std::string> mExtensions;

        bool mAutoLoad;
    };

}
}