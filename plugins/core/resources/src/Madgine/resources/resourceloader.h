#pragma once

#include "resourcebase.h"
#include "resourceloadercollector.h"

#include "Modules/threading/globalstorage.h"

#include "handle.h"

#include "resourceloaderbase.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Interfaces/filesystem/filewatcher.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Resources {

    MADGINE_RESOURCES_EXPORT ResourceLoaderBase &getLoaderByIndex(size_t i);
    MADGINE_RESOURCES_EXPORT void waitForIOThread();

    template <typename Interface>
    struct ResourceDataInfoBase {
        ResourceDataInfoBase(typename Interface::ResourceType *res)
            : mResource(res)
        {
        }

        ResourceDataInfoBase(ResourceDataInfoBase &&other) noexcept
            : mResource(other.mResource)
            , mRefCount(other.mRefCount.exchange(0))
        {
        }

        ResourceDataInfoBase &operator=(ResourceDataInfoBase &&other) noexcept
        {
            mResource = other.mResource;
            mRefCount = other.mRefCount.exchange(0);
            return *this;
        }

        typename Interface::ResourceType *resource() const
        {
            return mResource;
        }

        void incRef()
        {
            ++mRefCount;
        }

        bool decRef()
        {
            uint32_t oldCount = mRefCount--;
            assert(oldCount > 0);
            return oldCount > 1 || mPersistent;
        }

        void setPersistent(bool b)
        {
            mPersistent = b;
        }

        void setLoadingTask(Threading::TaskFuture<bool> task)
        {
            assert(!mLoadingTask.valid());
            assert(!mUnloadingTask.valid() || mUnloadingTask.is_ready());
            mUnloadingTask.reset();
            mLoadingTask = task;
        }

        void setUnloadingTask(Threading::TaskFuture<void> task)
        {
            assert(!mUnloadingTask.valid());
            assert(mLoadingTask.is_ready());
            mLoadingTask.reset();
            mUnloadingTask = task;
        }

        bool verify()
        {
            return mLoadingTask.valid() && mLoadingTask.is_ready() && mLoadingTask && !mUnloadingTask.valid();
        }

        Threading::TaskFuture<bool> loadingTask()
        {
            return mLoadingTask;
        }

        Threading::TaskFuture<void> unloadingTask()
        {
            return mUnloadingTask;
        }

    private:
        typename Interface::ResourceType *mResource;
        std::atomic<uint32_t> mRefCount = 0;
        bool mPersistent = false;
        Threading::TaskFuture<bool> mLoadingTask;
        Threading::TaskFuture<void> mUnloadingTask;
    };

    template <typename T, typename Data>
    struct PtrType {

        PtrType() = default;
        template <typename Data2>
        PtrType(std::unique_ptr<Data2> data)
            : mData(std::move(data))
        {
        }
        PtrType(PtrType &&) = default;

        ~PtrType()
        {
            T::unload(std::move(mData));
        }

        PtrType &operator=(PtrType &&) = default;

        operator Data *() const
        {
            return mData.get();
        }

        Data *operator->() const
        {
            return mData.get();
        }

        void reset()
        {
            mData.reset();
        }

        std::unique_ptr<Data> mData;
    };

    template <typename Interface>
    struct ResourceDataInfo : ResourceDataInfoBase<Interface> {
        using ResourceDataInfoBase<Interface>::ResourceDataInfoBase;
    };

    template <typename Loader>
    struct ResourceData {

        ResourceData(typename Loader::ResourceType *res)
            : mInfo(res)
        {
        }

        typename Loader::Data *verified(bool verified)
        {
            if (verified && !mInfo.verify())
                return nullptr;
            return &mData;
        }

        typename Loader::ResourceDataInfo mInfo;
        typename Loader::Data mData;
    };

    template <typename Loader>
    struct ResourceType : Loader::Interface::ResourceType {

        ResourceType(const std::string &name, const Filesystem::Path &path, typename Loader::Ctor ctor = {})
            : Loader::Interface::ResourceType(name, path)
            , mCtor(ctor ? std::move(ctor) : Loader::Interface::template toCtor<Loader>(&Loader::loadImpl))
        {
        }

        typename Loader::HandleType loadData()
        {
            return Loader::load(this);
        }

        typename Loader::Data *dataPtr()
        {
            return Loader::getDataPtr(loadData());
        }

        typename Loader::Ctor mCtor;

        typename Loader::Storage::template container_type<typename container_traits<typename Loader::DataContainer>::position_handle> mHolder;
    };

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename _Storage = Threading::GlobalStorage, typename _Base = ResourceLoaderCollector::Base>
    struct ResourceLoaderInterface : _Base {
        using Base = _Base;
        using Data = _Data;
        using Container = _Container;
        using Storage = _Storage;

        using ResourceDataInfo = ResourceDataInfo<T>;

        using DataContainer = typename replace<Container>::template type<ResourceData<T>>;

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;
        using PtrType = PtrType<T, Data>;
        using OriginalHandleType = HandleType;
        using OriginalPtrType = PtrType;

        using Ctor = Lambda<Threading::Task<bool>(T *, Data &, ResourceDataInfo &, Filesystem::FileEventType event)>;
        using UnnamedCtor = Lambda<Threading::Task<bool>(T *, Data &)>;

        struct ResourceType : ResourceLoaderBase::ResourceType {

            using ResourceLoaderBase::ResourceType::ResourceType;

            HandleType loadData()
            {
                return T::load(this);
            }

            Threading::TaskFuture<void> forceUnload()
            {
                return T::unload(this);
            }

            Data *dataPtr()
            {
                return T::getDataPtr(loadData());
            }

            using traits = container_traits<DataContainer>;

            typename Storage::template container_type<typename container_traits<DataContainer>::handle> mData;
        };

        template <typename Loader = T, typename C = void>
        static typename Loader::Ctor toCtor(C &&ctor)
        {
            static_assert(!std::is_same_v<C, typename Loader::Ctor>);
            return [ctor { std::forward<C>(ctor) }](T *loader, Data &data, ResourceDataInfo &info, Filesystem::FileEventType event) {
                return Threading::make_task(LIFT(TupleUnpacker::invoke), ctor, static_cast<Loader *>(loader), static_cast<typename Loader::Data &>(data), info, event);
            };
        }

        template <typename Loader = T>
        static typename Loader::Ctor toCtor(typename Loader::Ctor &&ctor)
        {
            return std::move(ctor);
        }

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

        using ResourceDataInfo = typename Interface::ResourceDataInfo;
        using ResourceType = ResourceType<T>;

        using DataContainer = typename replace<typename Base::Container>::template type<ResourceData<T>>;

        static_assert(!container_traits<DataContainer>::remove_invalidates_handles);

        using HandleType = Handle<T, typename container_traits<DataContainer>::handle>;
        //using PtrType = PtrType<T, Data>;

        using Ctor = Lambda<Threading::Task<bool>(T *, Data &, ResourceDataInfo &, Filesystem::FileEventType event)>;

        using Base::Base;

        static T &getSingleton()
        {
            return static_cast<T &>(getLoaderByIndex(component_index<T>()));
        }

        static HandleType load(std::string_view name, T *loader = nullptr)
        {
            if (name.empty())
                return {};
            if (!loader)
                loader = &getSingleton();
            ResourceType *res = get(name, loader);
            if (!res)
                return {};
            return load(res, Filesystem::FileEventType::FILE_CREATED, loader);
        }

        static ResourceType *get(std::string_view name, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();

            waitForIOThread();

            auto it = loader->mResources.find(name);
            if (it != loader->mResources.end())
                return &it->second;
            else
                return nullptr;
        }

        static ResourceDataInfo *getInfo(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return &(*loader->mData)[handle.mData].mInfo;
            } else {
                return &handle.mData->mInfo;
            }
        }

        template <typename C = Ctor>
        static ResourceType *getOrCreateManual(std::string_view name, const Filesystem::Path &path = {}, C &&ctor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            assert(name != ResourceBase::sUnnamed);
            return &loader->mResources.try_emplace(
                                          std::string { name }, std::string { name }, path, Interface::toCtor(std::forward<C>(ctor)))
                        .first->second;
        }

        static HandleType create(ResourceType *resource, Filesystem::FileEventType event = Filesystem::FileEventType::FILE_CREATED, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            assert(event == Filesystem::FileEventType::FILE_CREATED || handle);
            if (!handle || event != Filesystem::FileEventType::FILE_CREATED) {
                if (event == Filesystem::FileEventType::FILE_CREATED || !loader->mSettings.mInplaceReload) {
                    if (!loader)
                        loader = &getSingleton();
                    typename container_traits<DataContainer>::iterator it = container_traits<DataContainer>::emplace(*loader->mData, loader->mData->end(), resource);
                    *resource->mHolder = container_traits<DataContainer>::toPositionHandle(*loader->mData, it);
                    handle = container_traits<DataContainer>::toHandle(*loader->mData, *resource->mHolder);
                    *resource->mData = (decltype(*resource->mData))handle.mData;
                }
            }
            return handle;
        }

        static HandleType load(ResourceType *resource, Filesystem::FileEventType event = Filesystem::FileEventType::FILE_CREATED, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };

            if (!handle || event != Filesystem::FileEventType::FILE_CREATED) {
                if (!loader)
                    loader = &getSingleton();
                handle = create(resource, event, loader);

                ResourceDataInfo &info = *getInfo(handle, loader);
                info.setLoadingTask(loader->queueLoading(resource->mCtor(loader, *getDataPtr(handle, loader, false), info, event)));
            }

            return handle;
        }

        static Threading::TaskFuture<void> unload(ResourceType *resource, T *loader = nullptr)
        {
            HandleType handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            return unload(handle, loader);
        }

        static Threading::TaskFuture<void> unload(const HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return {};
            if (!loader)
                loader = &getSingleton();

            ResourceDataInfo &info = *getInfo(handle, loader);
            Threading::TaskFuture<void> task = info.unloadingTask();

            if (!task.valid()) {
                ResourceType *resource = handle.resource();

                task = loader->queueUnloading(Threading::make_task(&T::unloadImpl, loader, *getDataPtr(handle, loader, false)));

                info.setUnloadingTask(task);
            }

            return task;
        }

        static void unload(std::unique_ptr<Data> ptr, T *loader = nullptr)
        {
            if (!ptr)
                return;
            if (!loader)
                loader = &getSingleton();

            Threading::TaskFuture<void> task = loader->queueUnloading(Threading::make_task(&T::unloadImpl, loader, *ptr));

            auto cleanup = [ptr { std::move(ptr) }]() {};
            if (task.is_ready()) {
                cleanup();
            } else {
                task.then(std::move(cleanup), loader->loadingTaskQueue());
            }
        }

        static void resetHandle(const HandleType &handle, T *loader = nullptr)
        {
            assert(handle);
            if (!handle.info()->decRef()) {
                if (!loader)
                    loader = &getSingleton();

                Threading::TaskFuture<void> task = unload(handle, loader);

                ResourceType *resource = handle.resource();
                auto cleanup = [&data { *loader->mData }, handle { *resource->mHolder }]() {
                    typename container_traits<DataContainer>::iterator it = container_traits<DataContainer>::toIterator(data, handle);
                    data.erase(it);
                };
                if (task.is_ready()) {
                    cleanup();
                } else {
                    task.then(std::move(cleanup),
                        loader->loadingTaskQueue());
                }

                *resource->mData = {};
                *resource->mHolder = {};

                //if (resource->name() == ResourceBase::sUnnamed)
                //    delete resource;
            }
        }

        template <typename C = Ctor>
        static HandleType loadManual(std::string_view name, const Filesystem::Path &path = {}, C &&ctor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();
            return load(getOrCreateManual(
                            name, path, std::forward<C>(ctor),
                            loader),
                Filesystem::FileEventType::FILE_CREATED, loader);
        }

        static typename Interface::PtrType createUnnamed()
        {
            return std::make_unique<Data>();
        }

        template <typename C>
        static Threading::Task<bool> loadUnnamedTask(typename Interface::PtrType &ptr, C &&ctor)
        {
            PtrType ptr = createUnnamed();
            return Threading::make_task(std::forward<C>(ctor), *ptr);
        }

        template <typename C>
        static Threading::TaskFuture<bool> loadUnnamed(typename Interface::PtrType &ptr, C &&ctor, T *loader = nullptr)
        {
            if (!loader)
                loader = &getSingleton();

            return loader->queueLoading(loadUnnamedTask(ptr, std::forward<C>(ctor)));
        }

        static Data *getDataPtr(const HandleType &handle, T *loader = nullptr, bool verified = true)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &getSingleton();
                return (*loader->mData)[handle.mData].verified(verified);
            } else {
                return handle.mData->verified(verified);
            }
        }

        std::pair<ResourceBase *, bool> addResource(const Filesystem::Path &path, std::string_view name = {}) override
        {
            std::string actualName { name.empty() ? path.stem() : name };
            auto pib = mResources.try_emplace(actualName, actualName, path);

            if (pib.second)
                this->resourceAdded(&pib.first->second);

            return std::make_pair(&pib.first->second, pib.second);
        }

        void updateResourceData(ResourceBase *resource) override
        {
            if (static_cast<T *>(this)->mSettings.mAutoReload) {
                ResourceType *res = static_cast<ResourceType *>(resource);
                if (*res->mData)
                    load(res, Filesystem::FileEventType::FILE_MODIFIED);
            }
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
            std::ranges::transform(mResources, std::back_inserter(result), [](std::pair<const std::string, ResourceType> &p) {
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
        std::vector<ResourceType> mUnnamedResources;

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

        static typename Base::HandleType load(std::string_view name, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadVImpl(name);
        }

        static typename Base::HandleType load(typename Base::ResourceType *resource, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadVImpl(resource);
        }

        static Threading::TaskFuture<void> unload(typename Base::ResourceType *resource, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->unloadVImpl(resource);
        }

        static void unload(std::unique_ptr<_Data> ptr, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->unloadVImpl(std::move(ptr));
        }

        static void resetHandle(const typename Base::HandleType &handle, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->resetHandleVImpl(handle);
        }

        template <typename C = typename Base::Ctor>
        static typename Base::HandleType loadManual(std::string_view name, const Filesystem::Path &path = {}, C &&ctor = {}, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            return loader->loadManualVImpl(
                name, path, Base::toCtor(std::forward<C>(ctor)));
        }

        template <typename C>
        static Threading::TaskFuture<bool> loadUnnamed(typename Base::PtrType &ptr, C &&ctor, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();

            return loader->queueLoading(loadUnnamedTask(ptr, std::forward<C>(ctor), loader));
        }

        template <typename C = typename Base::Ctor>
        static Threading::Task<bool> loadUnnamedTask(typename Base::PtrType &ptr, C &&ctor, T *loader = nullptr)
        {
            if (!loader)
                loader = &Base::getSingleton();
            ptr = loader->createUnnamedVImpl();
            return Threading::make_task(std::forward<C>(ctor), (T*)loader, *ptr);
        }

        static typename Base::Data *getDataPtr(const typename Base::HandleType &handle, T *loader = nullptr, bool verified = true)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<typename Base::DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &Base::getSingleton();
                return loader->getDataPtrVImpl(handle, verified);
            } else {
                return handle.mData->verified(verified);
            }
        }

        static typename Base::ResourceDataInfo *getInfo(const typename Base::HandleType &handle, T *loader = nullptr)
        {
            if (!handle)
                return nullptr;
            if constexpr (container_traits<typename Base::DataContainer>::has_dependent_handle) {
                if (!loader)
                    loader = &Base::getSingleton();
                return loader->getInfoVImpl(handle);
            } else {
                return &handle.mData->mInfo;
            }
        }

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = ResourceLoaderBase::resourceTypes();
            result.push_back(table<typename Base::ResourceType>);
            return result;
        }

        virtual typename Base::HandleType loadManualVImpl(std::string_view name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}) = 0;
        virtual typename Base::PtrType createUnnamedVImpl() = 0;
        virtual typename Base::HandleType loadVImpl(std::string_view name) = 0;
        virtual typename Base::HandleType loadVImpl(typename Base::ResourceType *resource) = 0;
        virtual Threading::TaskFuture<void> unloadVImpl(typename Base::ResourceType *resource) = 0;
        virtual void unloadVImpl(std::unique_ptr<_Data> ptr) = 0;
        virtual void resetHandleVImpl(const typename Base::HandleType &handle) = 0;
        virtual typename Base::Data *getDataPtrVImpl(const typename Base::HandleType &handle, bool verified) = 0;
        virtual typename Base::ResourceDataInfo *getInfoVImpl(const typename Base::HandleType &handle) = 0;
    };

    template <typename T, typename _Data, typename _Base>
    struct VirtualResourceLoaderImpl : VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, _Base>>, _Base> {

        using Data = _Data;
        using Base = _Base;

        using Self = VirtualUniqueComponentImpl<T, VirtualScope<T, ResourceLoaderImpl<T, Data, Base>>, _Base>;

        using Self::Self;

        virtual typename Base::OriginalHandleType loadManualVImpl(std::string_view name, const Filesystem::Path &path = {}, typename Base::Ctor ctor = {}) override
        {
            return Self::loadManual(name, path, std::move(ctor), static_cast<T *>(this));
        }
        virtual typename Base::OriginalPtrType createUnnamedVImpl() override
        {
            return Self::createUnnamed();
        }
        virtual typename Base::OriginalHandleType loadVImpl(std::string_view name) override
        {
            return Self::load(name, static_cast<T *>(this));
        }
        virtual typename Base::OriginalHandleType loadVImpl(typename Base::ResourceType *resource) override
        {
            return Self::load(static_cast<typename Self::ResourceType *>(resource), Filesystem::FileEventType::FILE_CREATED, static_cast<T *>(this));
        }
        virtual Threading::TaskFuture<void> unloadVImpl(typename Base::ResourceType *resource) override
        {
            return Self::unload(static_cast<typename Self::ResourceType *>(resource), static_cast<T *>(this));
        }
        virtual void unloadVImpl(std::unique_ptr<typename Base::Data> data) override
        {
            return Self::unload(std::unique_ptr<Data> { static_cast<Data*>(data.release()) });
        }
        virtual void resetHandleVImpl(const typename Base::OriginalHandleType &handle) override
        {
            Self::resetHandle(handle, static_cast<T *>(this));
        }
        virtual typename Base::Data *getDataPtrVImpl(const typename Base::OriginalHandleType &handle, bool verified) override
        {
            return Self::getDataPtr(handle, static_cast<T *>(this), verified);
        }
        virtual typename Base::ResourceDataInfo *getInfoVImpl(const typename Base::OriginalHandleType &handle) override
        {
            return Self::getInfo(handle, static_cast<T *>(this));
        }
    };
}
}