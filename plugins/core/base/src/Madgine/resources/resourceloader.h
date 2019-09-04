#pragma once

#include "resource.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "resourceloadercollector.h"
#include "resourcemanager.h"
#include "Modules/reflection/decay.h"

namespace Engine {
namespace Resources {

    template <class T, class _Data, template <typename> typename ResourceKind>
    struct ResourceLoader : public UniqueComponent<T, ResourceLoaderCollector> {

        using Data = _Data;
        using ResourceType = ResourceKind<ResourceLoader<T, Data, ResourceKind>>;

        using UniqueComponent<T, ResourceLoaderCollector>::UniqueComponent;

        ResourceType *get(const std::string &name)
        {
            auto it = mResources.find(name);
            if (it != mResources.end())
                return &it->second;
            else
                return nullptr;
        }

        static std::shared_ptr<Data> load(const std::string &name)
        {
            return Resources::ResourceManager::getSingleton().load<T>(name);
        }

        virtual std::shared_ptr<Data> loadImpl(ResourceType *res) = 0;
        virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path) override
        {
            std::string name = path.stem();
            auto pib = mResources.try_emplace(name, this, path);

            if (pib.second)
                this->resourceAdded(&pib.first->second);

            return std::make_pair(&pib.first->second, pib.second);
        }

        typename std::map<std::string, ResourceType>::iterator begin()
        {
            return mResources.begin();
        }

        typename std::map<std::string, ResourceType>::iterator end()
        {
            return mResources.end();
        }

        std::map<std::string, ResourceType> mResources;
    };

}

template <class T, class Data, template <typename> typename ResourceKind>
struct decay<ResourceLoader<T, Data, ResourceKind>> {
    using type = T;
};

}