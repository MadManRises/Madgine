#pragma once

#include "resourcebase.h"
#include "resourceloadercollector.h"

#include "Modules/threading/globalstorage.h"

#include "handle.h"
#include "ptr.h"
#include "resource.h"

#include "resourceloaderbase.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Interfaces/filesystem/filewatcher.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Resources {

    MADGINE_RESOURCES_EXPORT ResourceLoaderBase &getLoaderByIndex(size_t i);
    MADGINE_RESOURCES_EXPORT void waitForIOThread();

    template <typename T, typename _Data, typename _Container = std::list<Placeholder<0>>, typename _Storage = Threading::GlobalStorage, typename _Base = ResourceLoaderCollector::Base>
    struct ResourceLoaderInterface : _Base {
        using Base = _Base;
        using Data = _Data;
        using Container = _Container;
        using Storage = _Storage;

        using ResourceDataInfo = ResourceDataInfo<T>;

        using DataContainer = typename replace<Container>::template type<ResourceData<T>>;

        using Handle = Handle<T, typename container_traits<DataContainer>::handle>;
        using Ptr = Ptr<T, Data>;
        using OriginalHandle = Handle;
        using OriginalPtr = Ptr;

        using Ctor = Lambda<Threading::Task<bool>(T *, Data &, ResourceDataInfo &, Filesystem::FileEventType event)>;
        using UnnamedCtor = Lambda<Threading::Task<bool>(T *, Data &)>;

        struct Resource : ResourceLoaderBase::Resource {

            using ResourceLoaderBase::Resource::Resource;

            Handle loadData()
            {
                return T::load(this);
            }

            /* Threading::TaskFuture<void> forceUnload()
            {
                return T::unload(this);
            }*/

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
            return static_cast<T &>(getLoaderByIndex(UniqueComponent::component_index<T>()));
        }
    };

    template <typename T, typename _Data, typename _Base = ResourceLoaderInterface<T, _Data>>
    struct ResourceLoaderImpl : _Base {

        using Interface = _Base;
        using Base = _Base;
        using Data = _Data;

        using ResourceDataInfo = typename Interface::ResourceDataInfo;
        using Resource = Resource<T>;

        using DataContainer = typename replace<typename Base::Container>::template type<ResourceData<T>>;

        static_assert(!container_traits<DataContainer>::remove_invalidates_handles);

        using Handle = Handle<T, typename container_traits<DataContainer>::handle>;
        //using Ptr = Ptr<T, Data>;

        using Ctor = Lambda<Threading::Task<bool>(T *, Data &, ResourceDataInfo &, Filesystem::FileEventType event)>;

        using Base::Base;

        static T &getSingleton()
        {
            return static_cast<T &>(getLoaderByIndex(UniqueComponent::component_index<T>()));
        }

        static Handle load(std::string_view name, T *loader = &getSingleton())
        {
            if (name.empty())
                return {};
            Resource *res = get(name, loader);
            if (!res)
                return {};
            return load(res, Filesystem::FileEventType::FILE_CREATED, loader);
        }

        static Resource *get(std::string_view name, T *loader = &getSingleton())
        {
            waitForIOThread();

            auto it = loader->mResources.find(name);
            if (it != loader->mResources.end())
                return &it->second;
            else
                return nullptr;
        }

        static ResourceDataInfo *getInfo(const Handle &handle, T *loader = nullptr)
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
        static Resource *getOrCreateManual(std::string_view name, const Filesystem::Path &path = {}, C &&ctor = {}, T *loader = &getSingleton())
        {
            assert(name != ResourceBase::sUnnamed);
            return &loader->mResources.try_emplace(
                                          std::string { name }, std::string { name }, path, Interface::template toCtor<T>(std::forward<C>(ctor)))
                        .first->second;
        }

        static Handle create(Resource *resource, Filesystem::FileEventType event = Filesystem::FileEventType::FILE_CREATED, T *loader = nullptr)
        {
            Handle handle { (typename container_traits<DataContainer>::handle) * resource->mData };
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

        static Handle load(Resource *resource, Filesystem::FileEventType event = Filesystem::FileEventType::FILE_CREATED, T *loader = nullptr)
        {
            Handle handle { (typename container_traits<DataContainer>::handle) * resource->mData };

            if (!handle || event != Filesystem::FileEventType::FILE_CREATED) {
                if (!loader)
                    loader = &getSingleton();
                handle = create(resource, event, loader);

                ResourceDataInfo &info = *getInfo(handle, loader);
                info.setLoadingTask(loader->queueLoading(resource->mCtor(loader, *getDataPtr(handle, loader, false), info, event)));
            }

            return handle;
        }

        static Threading::TaskFuture<void> unload(Resource *resource, T *loader = &getSingleton())
        {
            Handle handle { (typename container_traits<DataContainer>::handle) * resource->mData };
            return unload(handle, loader);
        }

        static Threading::TaskFuture<void> unload(const Handle &handle, T *loader = &getSingleton())
        {
            if (!handle)
                return {};

            ResourceDataInfo &info = *getInfo(handle, loader);
            Threading::TaskFuture<void> task = info.unloadingTask();

            if (!task.valid()) {
                task = loader->queueUnloading(Threading::make_task(&T::unloadImpl, (T *)loader, *getDataPtr(handle, loader, false)));

                info.setUnloadingTask(task);
            }

            return task;
        }

        static void unload(std::unique_ptr<Data> ptr, T *loader = &getSingleton())
        {
            if (!ptr)
                return;

            Threading::TaskFuture<void> task = loader->queueUnloading(Threading::make_task(&T::unloadImpl, (T *)loader, *ptr));

            auto cleanup = [ptr { std::move(ptr) }]() {};
            if (task.is_ready()) {
                cleanup();
            } else {
                task.then(std::move(cleanup), loader->loadingTaskQueue());
            }
        }

        static void resetHandle(const Handle &handle, T *loader = nullptr)
        {
            assert(handle);
            if (!handle.info()->decRef()) {
                if (!loader)
                    loader = &getSingleton();

                Threading::TaskFuture<void> task = unload(handle, loader);

                Resource *resource = handle.resource();
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
        static Handle loadManual(std::string_view name, const Filesystem::Path &path = {}, C &&ctor = {}, T *loader = &getSingleton())
        {
            return load(getOrCreateManual(
                            name, path, std::forward<C>(ctor),
                            loader),
                Filesystem::FileEventType::FILE_CREATED, loader);
        }

        static typename Interface::Ptr createUnnamed()
        {
            return std::make_unique<Data>();
        }

        template <typename C>
        static Threading::Task<bool> loadUnnamedTask(typename Interface::Ptr &ptr, C &&ctor)
        {
            ptr = createUnnamed();
            return Threading::make_task(std::forward<C>(ctor), *ptr);
        }

        template <typename C>
        static Threading::TaskFuture<bool> loadUnnamed(typename Interface::Ptr &ptr, C &&ctor, T *loader = &getSingleton())
        {
            return loader->queueLoading(loadUnnamedTask(ptr, std::forward<C>(ctor)));
        }

        static Data *getDataPtr(const Handle &handle, T *loader = nullptr, bool verified = true)
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
                Resource *res = static_cast<Resource *>(resource);
                if (*res->mData)
                    load(res, Filesystem::FileEventType::FILE_MODIFIED);
            }
        }

        typename std::map<std::string, Resource>::iterator begin()
        {
            return mResources.begin();
        }

        typename std::map<std::string, Resource>::iterator end()
        {
            return mResources.end();
        }

        virtual std::vector<ResourceBase*> resources() override
        {
            std::vector<ResourceBase*> result;
            std::ranges::transform(mResources, std::back_inserter(result), [](std::pair<const std::string, Resource> &p) {
                return &p.second;
            });
            return result;
        }

        virtual std::vector<std::pair<std::string_view, TypedScopePtr>> typedResources() override
        {
            std::vector<std::pair<std::string_view, TypedScopePtr>> result;
            std::ranges::transform(mResources, std::back_inserter(result), [](std::pair<const std::string, Resource> &p) {
                return std::make_pair(std::string_view { p.first }, &p.second);
            });
            return result;
        }

        virtual std::vector<const MetaTable *> resourceTypes() const override
        {
            std::vector<const MetaTable *> result = Base::resourceTypes();
            result.push_back(table<decayed_t<Resource>>);
            return result;
        }

        std::map<std::string, Resource, std::less<>> mResources;

        typename Base::Storage::template container_type<DataContainer> mData;
    };

    template <typename T, typename _Data, typename Container = std::list<Placeholder<0>>, typename Storage = Threading::GlobalStorage>
    struct ResourceLoader : ResourceLoaderComponent<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, ResourceLoaderInterface<T, _Data, Container, Storage>>>> {

        using ResourceLoaderComponent<T, VirtualScope<T, ResourceLoaderImpl<T, _Data, ResourceLoaderInterface<T, _Data, Container, Storage>>>>::ResourceLoaderComponent;
    };

}
}