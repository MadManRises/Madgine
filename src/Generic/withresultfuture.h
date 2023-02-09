#pragma once

#include "withresult.h"

namespace Engine {

template <typename T, typename R>
struct WithResultFuture {

    using type = T;

    WithResultFuture() = default;

    WithResultFuture(WithResultFuture &&) = default;

    WithResultFuture(std::future<WithResult<T, R>> future)
        : mFuture(std::move(future))
    {
    }

    WithResultFuture &operator=(WithResultFuture &&) = default;

    R get(T &result)
    {
        return mFuture.get().get(result);
    }

    operator T()
    {
        return mFuture.get();
    }

    bool is_ready() const
    {
        return mFuture.is_ready();
    }

private:
    std::future<WithResult<T, R>> mFuture;
};

template <typename R>
struct WithResultFuture<void, R> {

    using type = void;

    WithResultFuture() = default;

    WithResultFuture(WithResultFuture &&) = default;

    WithResultFuture(Void v)
        : mFuture(WithResult<void, R> {})
    {
    }

    WithResultFuture(std::future<WithResult<void, R>> future)
        : mFuture(std::move(future))
    {
    }

    WithResultFuture &operator=(WithResultFuture &&) = default;

    R get()
    {
        return mFuture.get().get();
    }

    bool is_ready() const
    {
        return mFuture.is_ready();
    }

private:
    std::future<WithResult<void, R>> mFuture;
};

}