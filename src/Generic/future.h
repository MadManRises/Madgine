#pragma once

#include "copy_traits.h"
#include "wrapreference.h"

#include <experimental/future>

namespace Engine {

#if !defined(__cpp_lib_experimental_future_continuations) || __cpp_lib_experimental_future_continuations < 201505

template <typename T>
std::future<T> make_ready_future(T &&t)
{
    std::promise<T> p;
    p.set_value(std::forward<T>(t));
    return p.get_future();
}

inline std::future<void> make_ready_future()
{
    std::promise<void> p;
    p.set_value();
    return p.get_future();
}

template <typename T, template <typename> typename Fut, typename F>
struct DeferredFuture;

template <typename T>
struct shared_future {

    shared_future() = default;
    shared_future(const shared_future<T> &) = default;
    shared_future(shared_future<T> &&) = default;

    shared_future<T> &operator=(shared_future<T> &&) = default;

    shared_future(std::shared_future<T> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::shared_future<T>>(mValue).valid());
    }

    template <typename U, typename F>
    shared_future(shared_future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, template <typename> typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<T, shared_future, F> then(F &&f) &&
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    const T &get() const
    {
        return std::visit(overloaded {
                              [](const std::shared_future<T> &f) -> const T & { return f.get(); },
                              [](const DeferredPtr &d) -> const T & { return std::move(*d).get(); } },
            std::move(mValue));
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const std::shared_future<T> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const DeferredPtr &d) { return d->isAvailable(); } },
            mValue);
    }

    shared_future<T> share() const
    {
        return std::visit(overloaded {
                              [](const std::shared_future<T> &f) -> shared_future<T> { return f; },
                              [](const DeferredPtr &d) -> shared_future<T> { return d; } },
            mValue);
    }

private:
    struct DeferredBase;
    using DeferredPtr = std::shared_ptr<DeferredBase>;

    struct DeferredBase {
        virtual ~DeferredBase() = default;
        virtual bool isAvailable() = 0;
        virtual T &get() = 0;
    };

    template <typename U, typename F>
    struct DeferredImpl : DeferredBase {

        DeferredImpl(shared_future<U> future, F &&f)
            : mFuture(std::move(future))
            , mF(std::forward<F>(f))
        {
        }

        bool isAvailable() override
        {
            return mResult || mFuture.isAvailable();
        }

        T &get() override
        {
            if (!mResult)
                mResult = std::move(mF)(mFuture.get());
            return mResult;
        }

        virtual DeferredPtr clone() override
        {
            return std::make_unique<DeferredImpl<U, F>>(mFuture.share(), tryCopy(mF));
        }

    private:
        std::optional<T> mResult;
        shared_future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

    shared_future(const DeferredPtr ptr)
        : mValue(ptr)
    {
        assert(std::get<DeferredPtr>(mValue));
    }

private:
    std::variant<std::shared_future<T>, DeferredPtr> mValue;
};

template <>
struct shared_future<void> {

    shared_future() = default;

    shared_future(shared_future<void> &&) = default;

    shared_future<void> &operator=(shared_future<void> &&) = default;

    shared_future(std::shared_future<void> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::shared_future<void>>(mValue).valid());
    }

    template <typename U, typename F>
    shared_future(shared_future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, template <typename> typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<void, shared_future, F> then(F &&f) &&
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const std::shared_future<void> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
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

        DeferredImpl(shared_future<U> future, F &&f)
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
        shared_future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

private:
    std::variant<std::shared_future<void>, std::unique_ptr<DeferredBase>> mValue;
};

template <typename T>
struct future {

    future() = default;

    future(future<T> &&) = default;

    future<T> &operator=(future<T> &&) = default;

    future(T value)
        : mValue(make_ready_future(std::forward<T>(value)))
    {
    }

    future(std::future<T> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::future<T>>(mValue).valid());
    }

    template <typename U, typename F>
    future(future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, template <typename> typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<T, future, F> then(F &&f)
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    T get()
    {
        return std::visit(overloaded {
                              [](std::future<T> &f) -> T { return f.get(); },
                              [](const DeferredPtr &d) -> T { return d->get(); } },
            mValue);
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
                              [](const std::future<T> &f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; },
                              [](const DeferredPtr &d) { return d->isAvailable(); } },
            mValue);
    }

    shared_future<T> share()
    {
        return std::visit(overloaded {
                              [](std::future<T> &f) -> shared_future<T> { return f.share(); },
                              [](DeferredPtr &d) -> shared_future<T> { return std::move(d); } },
            mValue);
    }

private:
    struct DeferredBase;
    using DeferredPtr = std::unique_ptr<DeferredBase>;

    struct DeferredBase {
        virtual ~DeferredBase() = default;
        virtual bool isAvailable() = 0;
        virtual T get() = 0;
    };

    template <typename U, typename F>
    struct DeferredImpl : DeferredBase {

        DeferredImpl(future<U> future, F &&f)
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
            return std::move(mF)(std::move(mFuture).get());
        }

    private:
        future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

    future(DeferredPtr &&ptr)
        : mValue(std::move(ptr))
    {
        assert(std::get<DeferredPtr>(mValue));
    }

private:
    std::variant<std::future<T>, DeferredPtr> mValue;
};

template <>
struct future<void> {

    future() = default;

    future(future<void> &&) = default;

    future<void> &operator=(future<void> &&) = default;

    future(std::future<void> future)
        : mValue(std::move(future))
    {
        assert(std::get<std::future<void>>(mValue).valid());
    }

    template <typename U, typename F>
    future(future<U> deferred, F &&f)
        : mValue(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename, template <typename> typename, typename>
    friend struct DeferredFuture;

    template <typename F>
    DeferredFuture<void, future, F> then(F &&f)
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    bool isAvailable() const
    {
        return std::visit(overloaded {
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

        DeferredImpl(future<U> future, F &&f)
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
        future<U> mFuture;
        std::remove_reference_t<F> mF;
    };

private:
    std::variant<std::future<void>, std::unique_ptr<DeferredBase>> mValue;
};

template <typename T, template <typename> typename Fut, typename F>
struct DeferredFuture {

    DeferredFuture(Fut<T> future, F &&f)
        : mFuture(std::move(future))
        , mF(std::forward<F>(f))
    {
    }

    std::invoke_result_t<F, T> get() &&
    {
        return mF(mFuture.get());
    }

    operator future<std::invoke_result_t<F, T>>() &&
    {
        return std::visit(overloaded {
                              [this](T &&t) { return future<std::invoke_result_t<F, T>> { mF(std::forward<T>(t)) }; },
                              [this](auto &&other) { return future<std::invoke_result_t<F, T>> { std::move(mFuture), std::move(mF) }; } },
            std::move(mFuture.mValue));
    }

    template <typename G>
    auto then(G &&g) &&
    {
        auto modified_f = [g { std::forward<G>(g) }, f { std::move(mF) }](auto &&arg) {
            return g(f(std::forward<decltype(arg)>(arg)));
        };
        return DeferredFuture<T, Fut, decltype(modified_f)> { std::move(mFuture), std::move(modified_f) };
    }

private:
    Fut<T> mFuture;
    F mF;
};

template <typename T, template <typename> typename Fut>
struct FutureWrapper : Fut<T> {
    using Fut<T>::Fut;

    FutureWrapper(Fut<T> f)
        : Fut<T>(std::move(f))
    {
    }

    FutureWrapper(T value)
        : Fut<T>(make_ready_future(std::forward<T>(value)))
    {
    }

    operator decltype(std::declval<Fut<T>>().get())()
    {
        if (!this->isAvailable())
            throw 0;
        return this->get();
    }

    template <typename U, typename = std::enable_if_t<std::is_constructible_v<U, T>>>
    operator FutureWrapper<U, future>()
    {
        return this->then([](T &&t) { return U { std::forward<T>(t) }; });
    }
};

#    define FUTURE_NAMESPACE

#endif

template <template <typename> typename Fut>
struct FutureWrapper<void, Fut> : Fut<void> {
    using Fut<void>::Fut;

    FutureWrapper()
        : Fut<void>(FUTURE_NAMESPACE make_ready_future())
    {
    }
};

template <typename T>
using Future = FutureWrapper<T, FUTURE_NAMESPACE future>;

template <typename T>
using SharedFuture = FutureWrapper<T, FUTURE_NAMESPACE shared_future>;

}