#pragma once

namespace Engine {

template <typename T>
struct RefPointer {
    
    RefPointer(T *p = nullptr)
        : mPtr(p)
    {
    }

    operator T *()
    {
        return mPtr;
    }

    operator const T *() const
    {
        return mPtr;
    }

    T &operator*()
    {
        return *mPtr;
    }

    const T &operator*() const
    {
        return *mPtr;
    }

    T *operator->()
    {
        return mPtr;
    }

    const T *operator->() const
    {
        return mPtr;
    }

private:
    T *mPtr;
};

}