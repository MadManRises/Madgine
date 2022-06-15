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
            if constexpr (std::derived_from<Loader2, Loader> || std::derived_from<Loader, Loader2>) {
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

        auto operator<=>(const Handle<Loader, Data> &) const = default;

        const auto &operator*() const
        {
            return *Loader::getDataPtr(*this);
        }

        const auto operator->() const
        {
            return Loader::getDataPtr(*this);
        }

        operator const typename Loader::Data *() const
        {
            return Loader::getDataPtr(*this);
        }

        bool available() const
        {
            return *this && info()->verify();
        }

        const typename Loader::Data* getUnsafe() const {
            return Loader::getDataPtr(*this, nullptr, false);
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
            *this = Loader::load(name, loader);
            typename Loader::ResourceDataInfo *i = info();
            if (!i)
                return false;
            return i->loadingTask();
        }

        template <typename C = typename Loader::Ctor>
        Threading::TaskFuture<bool> create(std::string_view name, const Filesystem::Path &path = {}, C &&c = {}, Loader *loader = nullptr)
        {
            *this = Loader::loadManual(name, path, std::forward<C>(c), loader);
            typename Loader::ResourceDataInfo *i = info();
            if (!i)
                return false;
            return i->loadingTask();
        }

        template <typename C = typename Loader::Ctor>
        Threading::Task<bool> createTask(std::string_view name, const Filesystem::Path &path = {}, C &&c = {}, Loader *loader = nullptr)
        {
            *this = Loader::loadManual(name, path, std::forward<C>(c), loader);
            typename Loader::ResourceDataInfo *i = info();
            if (!i)
                return false;
            return i->loadingTask();
        }

        void reset()
        {
            if (mData) {
                Loader::resetHandle(*this);
                mData = Data {};
            }
        }

        Data mData = {};
    };

}
}