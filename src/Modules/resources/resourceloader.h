#pragma once

#include "resourcebase.h"
#include "resourceloadercollector.h"
#include "resourcemanager.h"

#include "../threading/globalstorage.h"

#include "handle.h"

namespace Engine {
namespace Resources {

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename Storage = Threading::GlobalStorage, typename _Base = ResourceLoaderCollector::Base>
    struct ResourceLoaderImpl : _Base {

        using Base = _Base;
        using Data = _Data;
        using Container = _Container;

        struct ResourceType;

        using DataContainer = typename replace<Container>::template type<std::pair<ResourceType *, Data>>;

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;

        struct StorageUnit : Base::ResourceType {
            using Base::ResourceType::ResourceType;

            using traits = container_traits<DataContainer>;

            typename Storage::template container_type<typename container_traits<DataContainer>::handle> mData;
        };

        using Ctor = std::function<bool(T *, Data &, ResourceType *)>;
        using Dtor = std::function<void(T *, Data &, ResourceType *)>;

        using ResourceBaseType = std::conditional_t<std::is_same_v<ResourceLoaderCollector::Base, Base>, StorageUnit, typename Base::ResourceType>;

        struct ResourceType : ResourceBaseType {

            ResourceType(const Filesystem::Path &path, Ctor ctor = {}, Dtor dtor = {})
                : ResourceBaseType(path)
                , mCtor(ctor ? std::move(ctor) : [=](T *loader, Data &data, ResourceType *res) { return loader->loadImpl(data, res); })
                , mDtor(dtor ? std::move(dtor) : [=](T *loader, Data &data, ResourceType *res) { loader->unloadImpl(data, res); })
            {
            }

            HandleType loadData()
            {
                return load(this);
            }

            void unloadData()
            {
                unload(this);
            }

            Data *dataPtr()
            {
                return getDataPtr(loadData());
            }

            Ctor mCtor;
            Dtor mDtor;

            typename Storage::template container_type<typename container_traits<DataContainer>::position_handle> mHolder;
        };

        using Base::Base;

        static T &getSingleton()
        {
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static HandleType load(const std::string &name, bool persistent = false, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            if (name.empty())
                return {};
            return load(get(name, loader), persistent, loader);
        }

        static ResourceType *get(const std::string &name, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();

            auto it = loader->mResources.find(name);
            if (it != loader->mResources.end())
                return &it->second;
            else
                return nullptr;
        }

        static ResourceType *get(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return (*loader->mData)[handle.mData].first;
            } else {
                return handle.mData->first;
            }
        }

        static ResourceType *getOrCreateManual(const std::string &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return &loader->mResources.try_emplace(name, path, ctor, dtor).first->second;
        }

        static HandleType load(ResourceType *resource, bool persistent = false, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            if (!handle) {
                if (!loader)
                    loader = &getSingleton();
                *resource->mHolder = container_traits<DataContainer>::toPositionHandle(*loader->mData, container_traits<DataContainer>::emplace(*loader->mData, loader->mData->end(), std::pair<ResourceType *, Data> { resource, Data {} }).first);
                handle = container_traits<DataContainer>::toHandle(*loader->mData, *resource->mHolder);
                *resource->mData = (decltype(*resource->mData))handle.mData;
                resource->mCtor(loader, getData(handle, loader), resource);
            }
            return handle;
        }

        static void unload(ResourceType *resource, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            if (handle) {
                if (!loader)
                    loader = &getSingleton();
                resource->mDtor(loader, getData(handle, loader), resource);
            }
        }

        static HandleType loadManual(const std::string &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return load(getOrCreateManual(name, path, ctor, dtor, loader), loader);
        }

        static Data &getData(const HandleType &handle, T *loader = nullptr)
        {
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return (*loader->mData)[handle.mData].second;
            } else {
                return handle.mData->second;
            }
        }

        static Data *getDataPtr(ResourceType *resource, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            return getDataPtr(handle, loader);
        }

        static Data *getDataPtr(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            return &getData(handle, loader);
        }

        //bool load(Data &data, ResourceType *res) = 0;
        //void unload(Data &data) = 0;
        std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, const std::string &name = {}) override
        {
            std::string actualName = name.empty() ? path.stem() : name;
            auto pib = mResources.try_emplace(actualName, path);

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

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = Base::resourceTypes();
            result.push_back(&table<ResourceType>());
            return result;
        }

        std::map<std::string, ResourceType> mResources;

        typename Storage::template container_type<DataContainer> mData;
    };

    template <typename T, typename _Data, typename Container = std::list<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct ResourceLoader : public UniqueComponent<T, ResourceLoaderCollector, VirtualScope<T, ResourceLoaderImpl<T, _Data, Container, Storage>>> {

        using UniqueComponent<T, ResourceLoaderCollector, VirtualScope<T, ResourceLoaderImpl<T, _Data, Container, Storage>>>::UniqueComponent;
    };

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename _Storage = Threading::GlobalStorage>
    struct VirtualResourceLoaderBase : VirtualUniqueComponentBase<T, ResourceLoaderCollector> {

        using Data = _Data;
        using Container = _Container;
        using Storage = _Storage;

        struct ResourceType;

        using Ctor = std::function<bool(T *, Data &, ResourceType *)>;
        using Dtor = std::function<void(T *, Data &, ResourceType *)>;

        using DataContainer = typename replace<Container>::template type<std::pair<ResourceType *, Data>>;

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;
        using OriginalHandleType = HandleType;

        struct ResourceType : ResourceLoaderBase::ResourceType {

            using ResourceLoaderBase::ResourceType::ResourceType;

            HandleType loadData()
            {
                return load(this);
            }

            void unloadData()
            {
                return unload(this);
            }

            using traits = container_traits<DataContainer>;

            typename Storage::template container_type<typename container_traits<DataContainer>::handle> mData;
        };

        using VirtualUniqueComponentBase<T, ResourceLoaderCollector>::VirtualUniqueComponentBase;

        static T &getSingleton()
        {
            return Resources::ResourceManager::getSingleton().get<T>();
        }

        static HandleType load(const std::string &name, bool persistent = false, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return loader->loadVImpl(name, persistent);
        }

        static HandleType load(ResourceType *resource, bool persistent = false, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return loader->loadVImpl(resource, persistent);
        }

        static void unload(ResourceType *resource, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return loader->unloadVImpl(resource);
        }

        static HandleType loadManual(const std::string &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return loader->loadManualVImpl(name, path, std::move(ctor), std::move(dtor));
        }

        static Data &getData(const HandleType &handle, T *loader = nullptr)
        {
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return loader->getDataVImpl(handle);
            } else {
                return handle.mData->second;
            }
        }

        static Data *getDataPtr(ResourceType *resource, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            return getDataPtr(handle, loader);
        }

        static Data *getDataPtr(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            return &getData(handle, loader);
        }

        static ResourceType *get(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return loader->getVImpl(handle);
            } else {
                return handle.mData->first;
            }
        }

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = ResourceLoaderBase::resourceTypes();
            result.push_back(&table<ResourceType>());
            return result;
        }

        virtual HandleType loadManualVImpl(const std::string &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}) = 0;
        virtual HandleType loadVImpl(const std::string &name, bool persistent = false) = 0;
        virtual HandleType loadVImpl(ResourceType *resource, bool persistent = false) = 0;
        virtual void unloadVImpl(ResourceType *resource) = 0;
        virtual Data &getDataVImpl(const HandleType &handle) = 0;
        virtual ResourceType *getVImpl(const HandleType &handle) = 0;
    };

    template <typename T, typename _Data, typename _Base>
    struct VirtualResourceLoaderImpl : VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, typename _Base::Container, typename _Base::Storage, _Base>>, _Base> {

        using Data = _Data;
        using Base = _Base;

        using Self = VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, Data, typename Base::Container, typename Base::Storage, Base>>, _Base>;

        using Self::Self;

        virtual typename Base::OriginalHandleType loadManualVImpl(const std::string &name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}, typename Base::Dtor dtor = {}) override
        {
            return Self::loadManual(name, path, ctor, dtor, static_cast<T *>(this));
        }
        virtual typename Base::OriginalHandleType loadVImpl(const std::string &name, bool persistent = false) override
        {
            return Self::load(name, persistent, static_cast<T *>(this));
        }
        virtual typename Base::OriginalHandleType loadVImpl(typename Base::ResourceType *resource, bool persistent = false) override
        {
            return Self::load(static_cast<typename Self::ResourceType *>(resource), persistent, static_cast<T *>(this));
        }
        virtual void unloadVImpl(typename Base::ResourceType *resource) override
        {
            Self::unload(static_cast<typename Self::ResourceType *>(resource), static_cast<T *>(this));
        }
        virtual typename Base::Data &getDataVImpl(const typename Base::OriginalHandleType &handle) override
        {
            return Self::getData(handle, static_cast<T *>(this));
        }
        virtual typename Base::ResourceType *getVImpl(const typename Base::OriginalHandleType &handle) override
        {
            return Self::get(handle, static_cast<T *>(this));
        }
    };
}
}