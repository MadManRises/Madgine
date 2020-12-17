#pragma once

#include "resourcebase.h"
#include "resourceloadercollector.h"

#include "../threading/globalstorage.h"

#include "handle.h"

#include "resourceloaderbase.h"

#include "../uniquecomponent/uniquecomponent.h"
#include "../uniquecomponent/uniquecomponentcollector.h"

#include "../keyvalueutil/virtualscope.h"

namespace Engine {
namespace Resources {

    MODULES_EXPORT ResourceLoaderBase &getLoaderByIndex(size_t i);

    template <typename Loader>
    struct ResourceType : Loader::Interface::ResourceType {

        ResourceType(const Filesystem::Path &path, typename Loader::Ctor ctor = {}, typename Loader::Dtor dtor = {})
            : Loader::Interface::ResourceType(path)
            , mCtor(ctor ? std::move(ctor) : [](Loader *loader, typename Loader::Data &data, ResourceType *res) { return loader->loadImpl(data, res); })
            , mDtor(dtor ? std::move(dtor) : [](Loader *loader, typename Loader::Data &data, ResourceType *res) { loader->unloadImpl(data, res); })
        {
        }

        typename Loader::HandleType loadData()
        {
            return Loader::load(this);
        }

        void unloadData()
        {
            Loader::unload(this);
        }

        typename Loader::Data *dataPtr()
        {
            return Loader::getDataPtr(loadData());
        }

        typename Loader::Ctor mCtor;
        typename Loader::Dtor mDtor;

        typename Loader::Storage::template container_type<typename container_traits<typename Loader::DataContainer>::position_handle> mHolder;
    };

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename _Storage = Threading::GlobalStorage, typename _Base = ResourceLoaderCollector::Base>
    struct ResourceLoaderInterface : _Base {
        using Base = _Base;
        using Data = _Data;
        using Container = _Container;
        using Storage = _Storage;

        struct ResourceType;

        struct InfoBlock {
            ResourceType *mResource;
        };

        using DataContainer = typename replace<Container>::template type<std::pair<InfoBlock, Data>>;

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;
        using OriginalHandleType = HandleType;

        using Ctor = std::function<bool(T *, Data &, ResourceType *)>;
        using Dtor = std::function<void(T *, Data &, ResourceType *)>;

        struct ResourceType : ResourceLoaderBase::ResourceType {

            using ResourceLoaderBase::ResourceType::ResourceType;

            HandleType loadData()
            {
                return T::load(this);
            }

            void unloadData()
            {
                T::unload(this);
            }

            using traits = container_traits<DataContainer>;

            typename Storage::template container_type<typename container_traits<DataContainer>::handle> mData;
        };

        using Base::Base;

        static T &getSingleton()
        {
            return static_cast<T &>(getLoaderByIndex(component_index<T>()));
        }
    };

    template <typename T, typename _Data, typename _Base = ResourceLoaderInterface<T, _Data>>
    struct ResourceLoaderImpl : _Base {

        using Interface = _Base;
        using Base = _Base;
        using Data = _Data;

        using ResourceType = ResourceType<T>;

        using DataContainer = typename replace<typename Base::Container>::template type<std::pair<ResourceType *, Data>>;

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;

        using Ctor = std::function<bool(T *, Data &, ResourceType *)>;
        using Dtor = std::function<void(T *, Data &, ResourceType *)>;

        using Base::Base;

        static T &getSingleton()
        {
            return static_cast<T &>(getLoaderByIndex(component_index<T>()));
        }

        static HandleType load(const std::string_view &name, bool persistent = false, T *loader = nullptr)
        {
            if (name.empty())
                return {};
            if (!loader)
                loader = &getSingleton();
            ResourceType *res = get(name, loader);
            if (!res) {
                LOG_ERROR("No resource '" << name << "' available!");
                return {};
            }
            return load(res, persistent, loader);
        }

        static ResourceType *get(const std::string_view &name, T *loader = nullptr)
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

        static ResourceType *getOrCreateManual(const std::string_view &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return &loader->mResources.try_emplace(std::string { name }, path, ctor, dtor).first->second;
        }

        static HandleType load(ResourceType *resource, bool persistent = false, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            if (!handle) {
                if (!loader)
                    loader = &getSingleton();
                typename container_traits<DataContainer>::iterator it = container_traits<DataContainer>::emplace(*loader->mData, loader->mData->end(), std::piecewise_construct, std::make_tuple(resource), std::make_tuple());
                *resource->mHolder = container_traits<DataContainer>::toPositionHandle(*loader->mData, it);
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

        static HandleType loadManual(const std::string_view &name, const Filesystem::Path &path = {}, Ctor ctor = {}, Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return load(getOrCreateManual(name, path, ctor, dtor, loader), true, loader);
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
        std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, const std::string_view &name = {}) override
        {
            auto pib = mResources.try_emplace(std::string{name.empty() ? path.stem() : name }, path);

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

        virtual std::vector<std::pair<std::string_view, TypedScopePtr>> resources() override
        {
            std::vector<std::pair<std::string_view, TypedScopePtr>> result;
            std::transform(mResources.begin(), mResources.end(), std::back_inserter(result), [](std::pair<const std::string, ResourceType> &p) {
                return std::make_pair(std::string_view { p.first }, &p.second);
            });
            return result;
        }

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = Base::resourceTypes();
            result.push_back(table<decayed_t<ResourceType>>);
            return result;
        }

        std::map<std::string, ResourceType, std::less<>> mResources;

        typename Base::Storage::template container_type<DataContainer> mData;
    };

    template <typename T, typename _Data, typename Container = std::list<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct ResourceLoader : ResourceLoaderComponent<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, ResourceLoaderInterface<T, _Data, Container, Storage>>>> {

        using ResourceLoaderComponent<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, ResourceLoaderInterface<T, _Data, Container, Storage>>>>::ResourceLoaderComponent;
    };

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename _Storage = Threading::GlobalStorage>
    struct VirtualResourceLoaderBase : ResourceLoaderVirtualBase<T, ResourceLoaderInterface<T, _Data, _Container, _Storage>> {

        using Base = ResourceLoaderInterface<T, _Data, _Container, _Storage>;

        using ResourceLoaderVirtualBase<T, ResourceLoaderInterface<T, _Data, _Container, _Storage>>::ResourceLoaderVirtualBase;

        static typename Base::HandleType load(const std::string_view &name, bool persistent = false, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadVImpl(name, persistent);
        }

        static typename Base::HandleType load(typename Base::ResourceType *resource, bool persistent = false, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadVImpl(resource, persistent);
        }

        static void unload(typename Base::ResourceType *resource, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->unloadVImpl(resource);
        }

        static typename Base::HandleType loadManual(const std::string_view &name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}, typename Base::Dtor dtor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadManualVImpl(name, path, std::move(ctor), std::move(dtor));
        }

        static typename Base::Data &getData(const typename Base::HandleType &handle, T *loader = nullptr)
        {
            if constexpr (container_traits<typename Base::DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &Base::getSingleton();
                return loader->getDataVImpl(handle);
            } else {
                return handle.mData->second;
            }
        }

        static typename Base::Data *getDataPtr(typename Base::ResourceType *resource, T *loader = nullptr)
        {
            typename Base::HandleType handle { (typename container_traits<typename Base::DataContainer>::handle) * resource->mData };
            return getDataPtr(handle, loader);
        }

        static typename Base::Data *getDataPtr(const typename Base::HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            return &getData(handle, loader);
        }

        static typename Base::ResourceType *get(const typename Base::HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<typename Base::DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &Base::getSingleton();
                return loader->getVImpl(handle);
            } else {
                return handle.mData->first.mResource;
            }
        }

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = ResourceLoaderBase::resourceTypes();
            result.push_back(table<typename Base::ResourceType>);
            return result;
        }

        virtual typename Base::HandleType loadManualVImpl(const std::string_view &name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}, typename Base::Dtor dtor = {}) = 0;
        virtual typename Base::HandleType loadVImpl(const std::string_view &name, bool persistent = false) = 0;
        virtual typename Base::HandleType loadVImpl(typename Base::ResourceType *resource, bool persistent = false) = 0;
        virtual void unloadVImpl(typename Base::ResourceType *resource) = 0;
        virtual typename Base::Data &getDataVImpl(const typename Base::HandleType &handle) = 0;
        virtual typename Base::ResourceType *getVImpl(const typename Base::HandleType &handle) = 0;
    };

    template <typename T, typename _Data, typename _Base>
    struct VirtualResourceLoaderImpl : VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, _Base>>, _Base> {

        using Data = _Data;
        using Base = _Base;

        using Self = VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, Data, Base>>, _Base>;

        using Self::Self;

        virtual typename Base::OriginalHandleType loadManualVImpl(const std::string_view &name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}, typename Base::Dtor dtor = {}) override
        {
            return Self::loadManual(name, path, ctor, dtor, static_cast<T *>(this));
        }
        virtual typename Base::OriginalHandleType loadVImpl(const std::string_view &name, bool persistent = false) override
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