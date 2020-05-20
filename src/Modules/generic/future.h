#pragma once

namespace Engine {

template <typename T>
struct Future {

    Future() = default;

    Future(T value)
    {
        std::promise<T> p;
        p.set_value(value);
        mFuture = p.get_future();
    }

    Future(std::future<T> future)
        : mFuture(std::move(future))
    {
    }

    template <typename U, typename F>
    Future(Future<U> deferred, F &&f)
        : mDeferred(std::make_unique<DeferredImpl<U, F>>(std::move(deferred), std::forward<F>(f)))
    {
    }

    template <typename F>
    Future<std::invoke_result_t<F&&, T>> then(F &&f)
    {
        return { std::move(*this), std::forward<F>(f) };
    }

    operator T()
    {
        if (!isAvailable())
            throw 0;
        return get();
    }

    T operator->()
    {
        if (!isAvailable())
            throw 0;
        return get();
    }

    T get()
    {
        assert(mFuture.valid() != bool(mDeferred));
        if (mFuture.valid()) {
            return mFuture.get();
        } else if (mDeferred) {
            return mDeferred->get();
        } else {
            throw 0;
        }
    }

    bool isAvailable() const
    {
        if (mFuture.valid()) {
            return mFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
        } else {
            return mDeferred->isAvailable();
        }
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
    std::future<T> mFuture;
    std::unique_ptr<DeferredBase> mDeferred;
};

}