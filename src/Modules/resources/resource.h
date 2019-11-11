#pragma once

#include "resourcebase.h"

namespace Engine {
namespace Resources {

    template <typename Data>
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
    };

    template <typename Data>
    class Resource : public ResourceBase {
        //Resources:: namespace fixes weird issue on MSVC. DONT REMOVE!
        using Loader = ResourceLoaderImpl<Data, Resources::Resource>;

        using Ctor = std::function<std::shared_ptr<Data>(Resource<Data>*)>;

    public:
        Resource(Loader* loader, Filesystem::Path path = {})
            : ResourceBase(std::move(path))
            , mCtor([=](Resource<Data>* res) { return loader->loadImpl(res); })
        {
        }

        Resource(Ctor ctor)
            : mCtor(std::move(ctor))
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
        ResourceData<Data> mData;
        Ctor mCtor;
    };

    template <typename Data>
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

}
}