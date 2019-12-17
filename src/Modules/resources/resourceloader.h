#pragma once

#include "../reflection/decay.h"
#include "../uniquecomponent/uniquecomponent.h"
#include "resource.h"
#include "resourceloadercollector.h"
#include "resourcemanager.h"

#include "../generic/templates.h"

#include "../threading/globalstorage.h"

namespace Engine {
namespace Resources {

    template <typename _Data, typename Container = std::vector<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct ResourceLoaderImpl : ResourceLoaderBase {

        using Data = _Data;
        using ResourceType = Resource<Data, Container, Storage>;

        using ResourceLoaderBase::ResourceLoaderBase;

        ResourceType *get(const std::string &name)
        {
            auto it = mResources.find(name);
            if (it != mResources.end())
                return &it->second;
            else
                return nullptr;
        }

        ResourceType *getOrCreateManual(const std::string &name, const Filesystem::Path &path = {}, std::function<std::shared_ptr<Data>(ResourceType *)> ctor = {})
        {
            return &mResources.try_emplace(name, this, path, ctor).first->second;
        }

        virtual bool loadImpl(Data &data, ResourceType *res) = 0;
        virtual void unloadImpl(Data &data) = 0;
        virtual std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, const std::string &name = {}) override
        {
            std::string actualName = name.empty() ? path.stem() : name;
            auto pib = mResources.try_emplace(actualName, this, path);

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

    template <class T, class _Data, typename Container = std::vector<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct ResourceLoader : public UniqueComponent<T, ResourceLoaderCollector, ResourceLoaderImpl<_Data, Container, Storage>> {

        using UniqueComponent<T, ResourceLoaderCollector, ResourceLoaderImpl<_Data, Container, Storage>>::UniqueComponent;

        static T &getSingleton()
        {
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static std::shared_ptr<_Data> load(const std::string &name, bool persistent = false)
        {
            return Resources::ResourceManager::getSingleton().load<T>(name, persistent);
        }
    };

    template <typename T, class _Data, typename Container = std::vector<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct VirtualResourceLoaderBase : VirtualUniqueComponentBase<T, ResourceLoaderCollector, ResourceLoaderImpl<_Data, Container, Storage>> {
        using VirtualUniqueComponentBase<T, ResourceLoaderCollector, ResourceLoaderImpl<_Data, Container, Storage>>::VirtualUniqueComponentBase;

        static T &getSingleton()
        {            
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static std::shared_ptr<_Data> load(const std::string &name, bool persistent = false)
        {
            return Resources::ResourceManager::getSingleton().load<T>(name, persistent);
        }
    };

    template <typename T, typename Base>
    struct VirtualResourceLoaderImpl : VirtualUniqueComponentImpl<T, Base> {
        using VirtualUniqueComponentImpl<T, Base>::VirtualUniqueComponentImpl;

        static T &getSingleton()
        {
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static std::shared_ptr<typename Base::Data> load(const std::string &name, bool persistent = false)
        {
            return Resources::ResourceManager::getSingleton().load<T>(name, persistent);
        }
    };

}
}