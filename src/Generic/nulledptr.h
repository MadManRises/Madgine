#pragma once

namespace Engine {

template <typename T>
struct NulledPtr {
    NulledPtr() = default;
    NulledPtr(T *ptr)
        : mPtr(ptr)
    {
    }
    ~NulledPtr()
    {
        mPtr = nullptr;
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