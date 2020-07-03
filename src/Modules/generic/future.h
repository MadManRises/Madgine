#pragma once

#include "wrapreference.h"

namespace Engine {

    template <typename T, typename F>
struct DeferredFuture;

template <typename T>
struct Future {

    Future() = default;

    Future(T value)
        : mValue(std::move(value))
    {
    }

    Future(std::future<T> future)
        : mValue(std::move(future))
    {
    }

    template <typename U, typename F>
    Future(Future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<T, F> then(F &&f)
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    operator T()
    {
        if (!isAvailable())
            throw 0;
        return get();
    }

    T get()
    {
        return std::visit(overloaded {
                              [](const T &t) { return t; },
                              [](std::future<T> &f) { return f.get(); },
                              [](const std::unique_ptr<DeferredBase> &d) { return d->get(); } },
            mValue);
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const T &t) { return true; },
                              [](const std::future<T> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const std::unique_ptr<DeferredBase> &d) { return d->isAvailable(); } },
            mValue);
    }

private:
    struct DeferredBase {
        virtual ~DeferredBase() = default;
        virtual bool isAvailable() = 0;
        virtual T get() = 0;
    };

    template <typename U, typename F>
    struct DeferredImpl : DeferredBase {

        DeferredImpl(Future<U> future, F &&f)
            : mFuture(std::move(future))
            , mF(std::forward<F>(f))
        {
        }

        bool isAvailable() override
        {
            return mFuture.isAvailable();
        }

        T get() override
        {
            return mF(mFuture.get());
        }

    private:
        Future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

private:
    std::variant<wrap_reference_t<T>, std::future<T>, std::unique_ptr<DeferredBase>> mValue;
};

template <typename T, typename F>
struct DeferredFuture {

    DeferredFuture(Future<T> future, F &&f)
        : mFuture(std::move(future))
        , mF(std::forward<F>(f))
    {
    }

    std::invoke_result_t<F, T> get()
    {
        return mF(mFuture.get());
    }

    operator Future<std::invoke_result_t<F, T>>() &&
    {
        return std::visit(overloaded {
                              [this](T &&t) { return Future<std::invoke_result_t<F, T>> { mF(t) }; },
                              [this](auto &&other) { return Future<std::invoke_result_t<F, T>> { std::move(mFuture), std::move(mF) }; } },
            std::move(mFuture.mValue));
    }

    operator std::invoke_result_t<F, T>() {
        return get();
    }

private:
    Future<T> mFuture;
    F mF;
};

}