#pragma once

#include "Modules/reflection/decay.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "resource.h"
#include "resourceloadercollector.h"
#include "resourcemanager.h"

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

        static T &getSingleton()
        {
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static std::shared_ptr<Data> load(const std::string &name, bool persistent = false)
        {
            return Resources::ResourceManager::getSingleton().load<T>(name, persistent);
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

        virtual std::vector<std::pair<std::string, TypedScopePtr>> resources() override
        {
            std::vector<std::pair<std::string, TypedScopePtr>> result;
            std::transform(mResources.begin(), mResources.end(), std::back_inserter(result), [](std::pair<const std::string, ResourceType> &p) {
                return std::make_pair(p.first, &p.second);
            });
            return result;
        }

        virtual const MetaTable *resourceType() const override
        {
            return &table<ResourceType>();
        }

        std::map<std::string, ResourceType> mResources;
    };

}

template <class T, class Data, template <typename> typename ResourceKind>
struct decay<ResourceLoader<T, Data, ResourceKind>> {
    using type = T;
};

}