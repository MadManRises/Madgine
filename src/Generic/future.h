#pragma once

#include "copy_traits.h"
#include "wrapreference.h"

namespace Engine {

template <typename T, typename F>
struct DeferredFuture;

template <typename T>
struct Future {

    Future() = default;

    Future(Future<T> &&) = default;

    Future<T> &operator=(Future<T> &&) = default;

    Future(T value)
        : mValue(std::move(value))
    {
    }

    Future(std::future<T> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::future<T>>(mValue).valid());
    }

    Future(std::shared_future<T> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::shared_future<T>>(mValue).valid());
    }

    template <typename U, typename F>
    Future(Future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<T, F> then(F &&f) &&
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    template <typename U, typename = std::enable_if_t<std::is_constructible_v<U, T>>>
    operator Future<U>() &&
    {
        return std::move(*this).then([](T &&t) { return U { std::forward<T>(t) }; });
    }

    operator const T &()
    {
        if (!isAvailable())
            throw 0;
        return get();
    }

    const T &get() &
    {
        if (!std::holds_alternative<T>(mValue)) {
            mValue.template emplace<T>(std::visit(overloaded {
                                                      [](const T &t) -> T { throw 0; },
                                                      [](std::future<T> &f) -> T { return f.get(); },
                                                      [](std::shared_future<T> &f) -> T { return f.get(); },
                                                      [](const DeferredPtr &d) -> T { return std::move(*d).get(); } },
                mValue));
        }
        return std::get<0>(mValue);
    }

    T get() &&
    {
        return std::visit(overloaded {
                              [](T &&t) -> T { return std::move(t); },
                              [](std::future<T> &&f) -> T { return std::move(f).get(); },
                              [](std::shared_future<T> &&f) -> T { return std::move(f).get(); },
                              [](const DeferredPtr &d) -> T {
                                  return std::move(*d).get();
                              } },
            std::move(mValue));
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const T &t) { return true; },
                              [](const std::future<T> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const std::shared_future<T> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const DeferredPtr &d) { return d->isAvailable(); } },
            mValue);
    }

    Future<T> share()
    {
        if (std::holds_alternative<std::future<T>>(mValue)) {
            std::future<T> f { std::move(std::get<std::future<T>>(mValue)) };
            mValue.template emplace<std::shared_future<T>>(std::move(f));
        }
        return std::visit(overloaded {
                              [](const T &t) -> Future<T> { return t; },
                              [](const std::future<T> &f) -> Future<T> { throw 0; },
                              [](const std::shared_future<T> &f) -> Future<T> { return f; },
                              [](const DeferredPtr &d) -> Future<T> { return d->clone(); } },
            mValue);
    }

private:
    struct DeferredBase;
    using DeferredPtr = std::unique_ptr<DeferredBase>;

    struct DeferredBase {
        virtual ~DeferredBase() = default;
        virtual bool isAvailable() = 0;
        virtual T get() && = 0;
        virtual DeferredPtr clone() = 0;
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

        T get() && override
        {
            return std::move(mF)(std::move(mFuture).get());
        }

        virtual DeferredPtr clone() override
        {
            return std::make_unique<DeferredImpl<U, F>>(mFuture.share(), tryCopy(mF));
        }

    private:
        Future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

    Future(DeferredPtr &&ptr)
        : mValue(std::move(ptr))
    {
        assert(std::get<DeferredPtr>(mValue));
    }

private:
    std::variant<wrap_reference_t<T>, std::future<T>, std::shared_future<T>, DeferredPtr> mValue;
};

template <>
struct Future<void> {

    Future() = default;

    Future(Future<void> &&) = default;

    Future<void> &operator=(Future<void> &&) = default;

    Future(std::future<void> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::future<void>>(mValue).valid());
    }

    template <typename U, typename F>
    Future(Future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<void, F> then(F &&f) &&
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const std::monostate &t) { return true; },
                              [](const std::future<void> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const std::unique_ptr<DeferredBase> &d) { return d->isAvailable(); } },
            mValue);
    }

private:
    struct DeferredBase {
        virtual ~DeferredBase() = default;
        virtual bool isAvailable() = 0;
        virtual void get() = 0;
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

        void get() override
        {
            mF(mFuture.get());
        }

    private:
        Future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

private:
    std::variant<std::monostate, std::future<void>, std::unique_ptr<DeferredBase>> mValue;
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
                              [this](T &&t) { return Future<std::invoke_result_t<F, T>> { mF(std::forward<T>(t)) }; },
                              [this](auto &&other) { return Future<std::invoke_result_t<F, T>> { std::move(mFuture), std::move(mF) }; } },
            std::move(mFuture.mValue));
    }

    operator std::invoke_result_t<F, T>()
    {
        return get();
    }

    template <typename G>
    auto then(G &&g) &&
    {
        auto modified_f = [g { std::forward<G>(g) }, f { std::move(mF) }](auto &&arg) {
            return g(f(std::forward<decltype(arg)>(arg)));
        };
        return DeferredFuture<T, decltype(modified_f)> { std::move(mFuture), std::move(modified_f) };
    }

private:
    Future<T> mFuture;
    F mF;
};
}