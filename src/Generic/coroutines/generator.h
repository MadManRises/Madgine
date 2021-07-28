#pragma once

#include "coroutine.h"

template <typename T>
struct Generator {

    struct promise_type {
        Generator get_return_object()
        {
            return Generator { std::experimental::coroutine_handle<promise_type>::from_promise(*this) };
        }
        // called by compiler first time co_yield occurs
        std::experimental::suspend_never initial_suspend()
        {
            return {};
        }

        // required for co_yield
        std::experimental::suspend_always yield_value(T &t)
        {
            mValue = &t;
            return {};
        }

        std::experimental::suspend_never return_void()
        {
            return {};
        }

        void unhandled_exception() { }

        std::experimental::suspend_always final_suspend() noexcept
        {
            return {};
        }

        T *mValue = nullptr;
    };

    Generator(std::experimental::coroutine_handle<promise_type> handle)
        : mHandle(handle)
    {
    }

    Generator(Generator<T> &&other)
        : mHandle(std::exchange(other.mHandle, nullptr))
    {
    }

    ~Generator()
    {
        reset();
    }

    Generator& operator=(Generator<T>&& other) {
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    const T &get()
    {
        return *mHandle.promise().mValue;
    }

    bool next()
    {
        mHandle.resume();
        return !mHandle.done();
    }

    bool done()
    {
        return mHandle.done();
    }

    void reset()
    {
        if (mHandle) {
            mHandle.destroy();
            mHandle = nullptr;
        }
    }

    void *release()
    {
        return std::exchange(mHandle, nullptr).address();
    }

    static Generator<T> fromAddress(void *address)
    {
        return { std::experimental::coroutine_handle<promise_type>::from_address(address) };
    }

    std::experimental::coroutine_handle<promise_type> mHandle;
};