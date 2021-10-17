#pragma once

#include "coroutine.h"

namespace Engine {

template <typename T = void>
struct CoroutineHandle {

    CoroutineHandle() = default;
    CoroutineHandle(const CoroutineHandle &) = delete;
    CoroutineHandle(CoroutineHandle &&other)
        : mHandle(std::exchange(other.mHandle, nullptr))
    {
    }
    CoroutineHandle(std::experimental::coroutine_handle<T> handle)
        : mHandle(handle)
    {
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    CoroutineHandle(std::experimental::coroutine_handle<U> handle)
        : CoroutineHandle(CoroutineHandle<U> { handle })
    {
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    CoroutineHandle(CoroutineHandle<U> &&handle)
        : mHandle(std::experimental::coroutine_handle<T>::from_promise(handle.release().promise()))
    {
    }
    ~CoroutineHandle()
    {
        reset();
    }

    CoroutineHandle &operator=(const CoroutineHandle &) = delete;
    CoroutineHandle &operator=(CoroutineHandle &&other)
    {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    std::experimental::coroutine_handle<T> release()
    {
        return std::exchange(mHandle, nullptr);
    }

    void reset()
    {
        if (mHandle) {
            mHandle.destroy();
            mHandle = nullptr;
        }
    }

    void resume()
    {
        mHandle.resume();
    }

    bool done()
    {
        return mHandle.done();
    }

    std::experimental::coroutine_handle<T> get()
    {
        return mHandle;
    }

    T &promise()
    {
        return mHandle.promise();
    }

    T *operator->()
    {
        return &promise();
    }

    explicit operator bool() const
    {
        return static_cast<bool>(mHandle);
    }

    static CoroutineHandle fromPromise(T &address)
    {
        return { std::experimental::coroutine_handle<T>::from_promise(address) };
    }

private:
    std::experimental::coroutine_handle<T> mHandle;
};

template <>
struct CoroutineHandle<void> {

    CoroutineHandle() = default;
    CoroutineHandle(const CoroutineHandle &) = delete;
    CoroutineHandle(CoroutineHandle &&other)
        : mHandle(std::exchange(other.mHandle, nullptr))
    {
    }
    CoroutineHandle(std::experimental::coroutine_handle<> handle)
        : mHandle(handle)
    {
    }
    template <typename U>
    CoroutineHandle(std::experimental::coroutine_handle<U> handle)
        : CoroutineHandle(CoroutineHandle<U> { handle })
    {
    }
    template <typename U>
    CoroutineHandle(CoroutineHandle<U> &&handle)
        : mHandle(handle.release())
    {
    }
    ~CoroutineHandle()
    {
        reset();
    }

    CoroutineHandle &operator=(const CoroutineHandle &) = delete;
    CoroutineHandle &operator=(CoroutineHandle &&other)
    {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    std::experimental::coroutine_handle<> release()
    {
        return std::exchange(mHandle, nullptr);
    }

    void reset()
    {
        if (mHandle) {
            mHandle.destroy();
            mHandle = nullptr;
        }
    }

    void resume()
    {
        mHandle.resume();
    }

    bool done()
    {
        return mHandle.done();
    }

    std::experimental::coroutine_handle<> get()
    {
        return mHandle;
    }

    explicit operator bool() const
    {
        return static_cast<bool>(mHandle);
    }

private:
    std::experimental::coroutine_handle<> mHandle;
};


}