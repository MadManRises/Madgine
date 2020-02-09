#pragma once

namespace Engine {
namespace Resources {

    template <typename Loader, typename Data>
    struct Handle {

        Handle(typename Loader::ResourceType *res)
        {
            *this = Loader::load(res);
        }

        Handle(Data data = {})
            : mData(data) {};

        template <typename Loader2, typename Data2>
        Handle(const Handle<Loader2, Data2> &other)
        {
            if constexpr (std::is_base_of_v<Loader2, Loader> || std::is_base_of_v<Loader, Loader2>) {
                mData = (Data)other.mData;
            } else {
                static_assert(dependent_bool<Loader2, false>::value, "Invalid conversion-type for Handle!");
            }
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
            return Loader::getData(*this);
        }

        auto operator-> () const
        {
            return Loader::getDataPtr(*this);
        }

        operator typename Loader::Data *() const
        {
            return Loader::getDataPtr(*this);
        }

        typename Loader::ResourceType *resource() const
        {
            return Loader::get(*this);
        }

        std::string name() const
        {
            typename Loader::ResourceType *res = resource();
            return res ? res->name() : "";
        }

        operator bool() const
        {
            return mData != Data {};
        }

        void load(const std::string &name, Loader *loader = nullptr)
        {
            *this = Loader::load(name, loader);
        }

        Data mData;
    };

}
}