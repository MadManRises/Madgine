#pragma once

#include "Modules/threading/taskfuture.h"

namespace Engine {
namespace Resources {

    template <typename Loader, typename Data>
    struct Handle {

        Handle() = default;

        Handle(const Handle &other)
            : mData(other.mData)
        {
            if (mData)
                info()->incRef();
        }

        Handle(Handle &&other)
            : mData(std::exchange(other.mData, {}))
        {
        }

        Handle(typename Loader::ResourceType *res)
        {
            *this = Loader::load(res);
        }

        Handle(Data data)
            : mData(data)
        {
            if (mData)
                info()->incRef();
        }

        ~Handle()
        {
            reset();
        }

        template <typename Loader2, typename Data2>
        Handle(const Handle<Loader2, Data2> &other)
        {
            if constexpr (std::is_base_of_v<Loader2, Loader> || std::is_base_of_v<Loader, Loader2>) {
                mData = (Data)other.mData;
                if (mData)
                    info()->incRef();
            } else {
                static_assert(dependent_bool<Loader2, false>::value, "Invalid conversion-type for Handle!");
            }
        }

        Handle &operator=(const Handle &other)
        {
            reset();
            mData = other.mData;
            if (mData)
                info()->incRef();
            return *this;
        }

        Handle &operator=(Handle &&other)
        {
            std::swap(mData, other.mData);
            return *this;
        }

        Handle &operator=(typename Loader::ResourceType *res)
        {
            return *this = Loader::load(res);
        }

        bool operator==(const Handle<Loader, Data> &other) const
        {
            return mData == other.mData;
        }

        bool operator!=(const Handle<Loader, Data> &other) const
        {
            return mData != other.mData;
        }

        bool operator<(const Handle<Loader, Data> &other) const
        {
            return mData < other.mData;
        }

        decltype(auto) operator*() const
        {
            return *Loader::getDataPtr(*this);
        }

        auto operator->() const
        {
            return Loader::getDataPtr(*this);
        }

        operator typename Loader::Data *() const
        {
            return Loader::getDataPtr(*this);
        }

        bool available() const
        {
            return *this && info()->verify();
        }

        typename Loader::ResourceDataInfo *info() const
        {
            return Loader::getInfo(*this);
        }

        typename Loader::ResourceType *resource() const
        {
            typename Loader::ResourceDataInfo *i = info();
            if (!i)
                return nullptr;
            return static_cast<typename Loader::ResourceType *>(i->resource());
        }

        std::string_view name() const
        {
            typename Loader::ResourceType *res = resource();
            return res ? res->name() : "";
        }

        explicit operator bool() const
        {
            return mData != Data {};
        }

        Threading::TaskFuture<bool> load(std::string_view name, Loader *loader = nullptr)
        {
            reset();
            *this = Loader::load(name, loader);
            typename Loader::ResourceDataInfo *i = info();
            if (!i)
                return false;
            return i->loadingTask();
        }

        void reset()
        {
            if (mData) {
                Loader::unload(*this);
                mData = Data {};
            }
        }

        Data mData = {};
    };

}
}