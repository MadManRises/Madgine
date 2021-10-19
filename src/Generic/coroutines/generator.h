#pragma once

#include "coroutine.h"
#include "handle.h"

namespace Engine {

template <typename T>
struct Generator {

    struct promise_type {
        Generator get_return_object()
        {
            return { CoroutineHandle<promise_type>::fromPromise(*this) };
        }

        std::suspend_never initial_suspend()
        {
            return {};
        }

        std::suspend_always yield_value(T &t)
        {
            mValue = &t;
            return {};
        }

        void return_void()
        {
        }

        void unhandled_exception() { }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        T *mValue = nullptr;
    };

    Generator(CoroutineHandle<promise_type> handle)
        : mHandle(std::move(handle))
    {
    }

    const T &get()
    {
        return *mHandle->mValue;
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

    CoroutineHandle<promise_type> release()
    {
        return std::move(mHandle);
    }

    CoroutineHandle<promise_type> mHandle;
};

}