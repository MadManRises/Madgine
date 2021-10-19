#pragma once

namespace Engine {

template <typename R, typename T, typename... Args>
struct LambdaImpl {

    static_assert(std::is_same_v<T, void>);

    struct Callable {
        virtual ~Callable() = default;
        virtual R call(Args... args) = 0;
    };

    template <typename F>
    struct CallableImpl : Callable {

        CallableImpl(F f)
            : mF(std::move(f))
        {
        }

        virtual R call(Args... args) override
        {
            if constexpr (std::is_same_v<R, void>)
                mF(std::forward<Args>(args)...);
            else
                return mF(std::forward<Args>(args)...);
        }

        F mF;
    };

    LambdaImpl() = default;

    template <typename F>
    LambdaImpl(F &&f)
        : mCallable(std::make_unique<CallableImpl<std::remove_reference_t<F>>>(std::forward<F>(f)))
    {
    }

    LambdaImpl(LambdaImpl &&) = default;

    LambdaImpl &operator=(LambdaImpl &&) = default;

    R operator()(Args... args)
    {
        return mCallable->call(std::forward<Args>(args)...);
    }

    explicit operator bool() const
    {
        return static_cast<bool>(mCallable);
    }

private:
    std::unique_ptr<Callable> mCallable;
};

template <typename F>
using Lambda = typename CallableTraits<F>::template instance<LambdaImpl>::type;

}