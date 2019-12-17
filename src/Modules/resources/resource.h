#pragma once

#include "resourcebase.h"

#include "../threading/workgroup.h"

#include "../keyvalue/container_traits.h"

namespace Engine {
namespace Resources {

    /*template <typename Data>
    struct ResourceData {
        void setPersistent(bool b)
        {
            if (b) {
                if (!mPtr && !mWeakPtr.expired()) {
                    mPtr = std::shared_ptr<Data>(mWeakPtr);
                }
            } else {
                mPtr.reset();
            }
        }

        template <typename Loader>
        std::shared_ptr<Data> load(Loader&& loader)
        {
            if (mPtr) {
                return mPtr;
            } else if (std::shared_ptr<Data> ptr = mWeakPtr.lock()) {
                return ptr;
            } else {
                ptr = std::forward<Loader>(loader)();
                mWeakPtr = ptr;
                mPtr = ptr;
                return ptr;
            }
        }

        bool unload()
        {
            mPtr.reset();
            bool b = mWeakPtr.expired();
            mWeakPtr.reset();
            return b;
        }

        bool isLoaded()
        {
            return !mWeakPtr.expired();
        }

        std::shared_ptr<Data> mPtr;
        std::weak_ptr<Data> mWeakPtr;
    };*/

    template <typename Data, typename Container, typename Storage>
    class Resource : public ResourceBase {        
        using Loader = ResourceLoaderImpl<Data, Container, Storage>;

        using Ctor = std::function<bool(Data &, Resource<Data, Container, Storage>*)>;

    public:
        Resource(Loader* loader, Filesystem::Path path = {}, Ctor ctor = {})
            : ResourceBase(std::move(path))
            , mCtor(ctor ? std::move(ctor) :
				[=](Resource<Data>* res) { return loader->loadImpl(res); })
        {
        }

        ~Resource()
        {
            if (!unload())
                LOG_WARNING("Deleted Resource \"" << name() << extension() << "\" still used. Memory not freed!");
        }

        std::shared_ptr<Data> data()
        {
            return mData.mPtr;
        }

        std::shared_ptr<Data> loadData()
        {
            return mData.load([this]() { return mCtor(this); });
        }

        bool load()
        {
            return loadData().operator bool();
        }

        bool unload()
        {
            return mData.unload();
        }

        void setPersistent(bool b)
        {
            ResourceBase::setPersistent(b);
            mData.setPersistent(b);
        }

    protected:
        struct ResourceData {
			//refCount
            typename container_traits<Container>::position_handle mHandle;
		};

		typename Storage::template container_type<ResourceData> mData;

        Ctor mCtor;
        Loader* mLoader;
    };

    /*template <typename Data>
    class ThreadLocalResource : public ResourceBase {
        //Resources:: namespace fixes weird issue on MSVC. DONT REMOVE!
        using Loader = ResourceLoaderImpl<Data, Resources::ThreadLocalResource>;

        using Ctor = std::function<std::shared_ptr<Data>(ThreadLocalResource<Data>*)>;

    public:
        ThreadLocalResource(Loader* loader, Filesystem::Path path = {})
            : ResourceBase(std::move(path))
            , mCtor([=](ThreadLocalResource<Data>* res) { return loader->loadImpl(res); })
        {
        }

        ThreadLocalResource(Ctor ctor)
            : mCtor(std::move(ctor))
        {
        }

        ~ThreadLocalResource()
        {
            for (auto& [thread, data] : mData) {
                if (!data.unload())
                    LOG_WARNING("Deleted thread-local Resource \"" << name() << extension() << "\" still used by thread " << thread << ". Memory not freed!");
            }
        }

        std::shared_ptr<Data> data()
        {
            return mData.at(std::this_thread::get_id()).mPtr;
        }

        std::shared_ptr<Data> loadData()
        {
            return mData[std::this_thread::get_id()].load([this]() { return mCtor(this); });
        }

        bool load()
        {
            return loadData().operator bool();
        }

        bool unload()
        {
            auto it = mData.find(std::this_thread::get_id());
            if (it == mData.end())
                return true;
            return it->second.unload();
        }

        bool isLoaded() const
        {
            auto it = mData.find(std::this_thread::get_id());
            if (it == mData.end())
                return false;
            return it->second.isLoaded();
        }

        void setPersistent(bool b)
        {
            ResourceBase::setPersistent(b);
            for (std::pair<const std::thread::id, ResourceData<Data>>& d : mData) {
                d.second.setPersistent(b);
            }
        }

    protected:
        std::map<std::thread::id, ResourceData<Data>> mData;
        Ctor mCtor;
    };

	template <typename Data>
    class WorkgroupLocalResource : public ResourceBase {
        //Resources:: namespace fixes weird issue on MSVC. DONT REMOVE!
        using Loader = ResourceLoaderImpl<Data, Resources::WorkgroupLocalResource>;

        using Ctor = std::function<std::shared_ptr<Data>(WorkgroupLocalResource<Data>*)>;

    public:
        WorkgroupLocalResource(Loader* loader, Filesystem::Path path = {})
            : ResourceBase(std::move(path))
            , mCtor([=](ThreadLocalResource<Data>* res) { return loader->loadImpl(res); })
        {
        }

        WorkgroupLocalResource(Ctor ctor)
            : mCtor(std::move(ctor))
        {
        }

        ~WorkgroupLocalResource()
        {
            for (auto& [workgroup, data] : mData) {
                if (!data.unload())
                    LOG_WARNING("Deleted workgroup-local Resource \"" << name() << extension() << "\" still used by workgroup \"" << workgroup->name() << "\". Memory not freed!");
            }
        }

        std::shared_ptr<Data> data()
        {
            return mData.at(&Threading::WorkGroup::self()).mPtr;
        }

        std::shared_ptr<Data> loadData()
        {
            return mData[&Threading::WorkGroup::self()].load([this]() { return mCtor(this); });
        }

        bool load()
        {
            return loadData().operator bool();
        }

        bool unload()
        {
            auto it = mData.find(&Threading::WorkGroup::self());
            if (it == mData.end())
                return true;
            return it->second.unload();
        }

        bool isLoaded() const
        {
            auto it = mData.find(&Threading::WorkGroup::self());
            if (it == mData.end())
                return false;
            return it->second.isLoaded();
        }

        void setPersistent(bool b)
        {
            ResourceBase::setPersistent(b);
            for (std::pair<const Threading::WorkGroup* const, ResourceData<Data>>& d : mData) {
                d.second.setPersistent(b);
            }
        }

    protected:
        std::map<const Threading::WorkGroup*, ResourceData<Data>> mData;
        Ctor mCtor;
    };*/

}
}