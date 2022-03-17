#pragma once

namespace Engine {

template <typename T>
struct NulledPtr {
    NulledPtr() = default;
    NulledPtr(T *ptr)
        : mPtr(ptr)
    {
    }
    NulledPtr(NulledPtr &&other)
        : mPtr(std::exchange(other.mPtr, nullptr))
    {
    }
    ~NulledPtr()
    {
        mPtr = nullptr;
    }

    NulledPtr& operator=(NulledPtr&& other) {
        mPtr = std::exchange(other.mPtr, nullptr);
        return *this;
    }

    operator T *() const
    {
        return mPtr;
    }
    T *operator->() const
    {
        return mPtr;
    }

private:
    T *mPtr = nullptr;
};

}