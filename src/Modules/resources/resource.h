#pragma once

#include "resourcebase.h"

namespace Engine {
namespace Resources {

    template <typename Data>
    class Resource : public ResourceBase {
        //Resources:: namespace fixes weird issue on MSVC. DONT REMOVE!
        using Loader = ResourceLoaderImpl<Data, Resources::Resource>;

    public:
        Resource(Loader* loader, const Filesystem::Path& path)
            : ResourceBase(path)
            , mLoader(loader)
        {
        }

        ~Resource()
        {
            if (!unload())
                LOG_WARNING("Deleted Resource \"" << name() << extension() << "\" still used. Memory not freed!");
        }

        std::shared_ptr<Data> data()
        {
            return mPtr;
        }

        std::shared_ptr<Data> loadData()
        {
            if (mPtr) {
                return mPtr;
            } else if (std::shared_ptr<Data> ptr = mWeakPtr.lock()) {
                return ptr;
            } else {
                ptr = mLoader->loadImpl(this);
                mWeakPtr = ptr;
                if (isPersistent())
                    mPtr = ptr;
                return ptr;
            }
        }

        bool load()
        {
            return loadData().operator bool();
        }

        bool unload()
        {
            mPtr.reset();
            bool b = mWeakPtr.expired();
            mWeakPtr.reset();
            return b;
        }

        void setPersistent(bool b)
        {
            ResourceBase::setPersistent(b);
            if (b) {
                if (!mPtr && !mWeakPtr.expired()) {
                    mPtr = std::shared_ptr<Data>(mWeakPtr);
                }
            } else {
                mPtr.reset();
            }
        }

    protected:
        std::shared_ptr<Data> mPtr;
        std::weak_ptr<Data> mWeakPtr;
        Loader* mLoader;
    };

    template <typename Data>
    class ThreadLocalResource : public ResourceBase {
		//Resources:: namespace fixes weird issue on MSVC. DONT REMOVE!
        using Loader = ResourceLoaderImpl<Data, Resources::ThreadLocalResource>;

    public:
        ThreadLocalResource(Loader* loader, const Filesystem::Path& path)
            : ResourceBase(path)
            , mLoader(loader)
        {
        }

        ~ThreadLocalResource()
        {
            for (auto& [thread, data] : mData) {
                data.mPtr.reset();
                if (!data.mWeakPtr.expired())
                    LOG_WARNING("Deleted thread-local Resource \"" << name() << extension() << "\" still used by thread " << thread << ". Memory not freed!");
            }
        }

        std::shared_ptr<Data> data()
        {
            return mData.at(std::this_thread::get_id()).mPtr;
        }

        std::shared_ptr<Data> loadData()
        {
            StorageData& d = mData[std::this_thread::get_id()];
            if (d.mPtr) {
                return d.mPtr;
            } else if (std::shared_ptr<Data> ptr = d.mWeakPtr.lock()) {
                return ptr;
            } else {
                ptr = mLoader->loadImpl(this);
                d.mWeakPtr = ptr;
                if (isPersistent())
                    d.mPtr = ptr;
                return ptr;
            }
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
            StorageData& d = it->second;
            d.mPtr.reset();
            bool b = d.mWeakPtr.expired();
            d.mWeakPtr.reset();
            return b;
        }

        bool isLoaded() const
        {
            auto it = mData.find(std::this_thread::get_id());
            if (it == mData.end())
                return false;
            return !it->second.mWeakPtr.expired();
        }

        void setPersistent(bool b)
        {
            ResourceBase::setPersistent(b);
            if (b) {
                for (std::pair<const std::thread::id, StorageData>& d : mData) {
                    if (!d.second.mPtr && !d.second.mWeakPtr.expired()) {
                        d.second.mPtr = std::shared_ptr<Data>(d.second.mWeakPtr);
                    }
                }
            } else {
                for (std::pair<const std::thread::id, StorageData>& d : mData) {
                    d.second.mPtr.reset();
                }
            }
        }

    protected:
        struct StorageData {
            std::shared_ptr<Data> mPtr;
            std::weak_ptr<Data> mWeakPtr;
        };

        std::map<std::thread::id, StorageData> mData;
        Loader* mLoader;
    };

}
}