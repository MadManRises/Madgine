#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct ApiMethod {
    typedef ValueType (*FPtr)(TypedScopePtr, const ArgumentList &);

    constexpr ApiMethod()
        : mFunction(nullptr)
        , mArgumentsCount(0)
        , mIsVariadic(false)
    {
    }

private:
    constexpr ApiMethod(FPtr f, size_t argumentsCount, bool isVariadic)
        : mFunction(f)
        , mArgumentsCount(argumentsCount)
        , mIsVariadic(isVariadic)
    {
    }

    template <auto F, typename R, typename T, typename... Args, size_t... I>
    static ValueType unpackHelper(T *t, const ArgumentList &args, std::index_sequence<I...>)
    {
        if constexpr (std::is_same_v<R, void>) {
            (t->*F)(args.at(I).as<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
            return {};
        } else {
            return (t->*F)(args.at(I).as<std::remove_cv_t<std::remove_reference_t<Args>>>()...);
        }
    }

    template <auto F, typename R, typename T, typename... Args>
    static ValueType unpack(TypedScopePtr scope, const ArgumentList &args)
    {
        T *t = scope.safe_cast<T>();
        return unpackHelper<F, R, T, Args...>(t, args, std::make_index_sequence<sizeof...(Args)>());
    }

    template <auto F, typename R, typename T, typename... Args>
    static ApiMethod wrapHelper(R (T::*f)(Args...))
    {
        return ApiMethod {
            &ApiMethod::unpack<F, R, T, Args...>,
            sizeof...(Args),
            false
        };
    }

public:
    template <auto F>
    static constexpr ApiMethod wrap()
    {
        return wrapHelper<F>(F);
    }

    bool operator==(const ApiMethod &other) const
    {
        return mFunction == other.mFunction;
    }

    operator bool() const
    {
        return mFunction != nullptr;
    }

    operator FPtr() const
    {
        return mFunction;
    }

    size_t arguments_count() const
    {
        return mArgumentsCount;
    }

private:
    FPtr mFunction;
    size_t mArgumentsCount;
    bool mIsVariadic;
};

}