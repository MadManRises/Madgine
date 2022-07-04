#pragma once

namespace Engine {
namespace Resources {

    template <typename T, typename Data>
    struct Ptr {

        Ptr() = default;
        template <typename Data2>
        Ptr(std::unique_ptr<Data2> data)
            : mData(std::move(data))
        {
        }
        Ptr(Ptr &&) = default;

        ~Ptr()
        {
            reset();
        }

        Ptr& operator=(Ptr&& other) {
            std::swap(mData, other.mData);
            return *this;
        }

        operator Data *() const
        {
            return mData.get();
        }

        Data *operator->() const
        {
            return mData.get();
        }

        template <typename C>
        Threading::TaskFuture<bool> create(C&& c, T *loader = &T::getSingleton()) {
            return T::loadUnnamed(*this, std::forward<C>(c), loader);
        }

        template <typename C>
        Threading::Task<bool> createTask(C&& c, T* loader = &T::getSingleton()) {
            return T::loadUnnamedTask(*this, std::forward<C>(c), loader);
        }

        void reset()
        {
            T::unload(std::move(mData));
            mData.reset();
        }

        std::unique_ptr<Data> mData;
    };


}    
}