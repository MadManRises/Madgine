#pragma once

namespace Engine {

template <typename F, typename R, typename T, typename... _Ty>
struct CallableType {

    typedef F type;
    typedef R return_type;
    typedef T class_type;
    typedef std::tuple<_Ty...> argument_types;
    typedef std::tuple<std::decay_t<_Ty>...> decay_argument_types;

    static constexpr size_t argument_count = sizeof...(_Ty) + (std::is_same_v<T, void> ? 0 : 1);

    template <template <typename, typename, typename...> typename C, typename... Args>
    struct instance {
        using type = C<Args..., R, T, _Ty...>;
    };
};

template <typename T>
struct StaticCallableMaker;

template <typename F, typename R, typename T, typename... _Ty>
struct StaticCallableMaker<CallableType<F, R, T, _Ty...>> {
    typedef CallableType<T, R, void, _Ty...> type;
};

template <typename T>
using makeStaticCallable = typename StaticCallableMaker<T>::type;

namespace __generic__impl__ {
    template <typename R, typename T, typename... _Ty>
    CallableType<R (T::*)(_Ty...), R, T, _Ty...> callableTypeDeducer(R (T::*f)(_Ty...));

    template <typename R, typename T, typename... _Ty>
    CallableType<R (T::*)(_Ty...) const, R, const T, _Ty...> callableTypeDeducer(R (T::*f)(_Ty...) const);

    template <typename R, typename T>
    CallableType<R(T::*), R &, T> callableTypeDeducer(R(T::*p));

    template <typename R, typename... _Ty>
    CallableType<R (*)(_Ty...), R, void, _Ty...> callableTypeDeducer(R (*f)(_Ty...));

    template <typename F, typename R = makeStaticCallable<decltype(callableTypeDeducer(&F::operator()))>>
    R callableTypeDeducer(const F &);
}

template <typename F>
using CallableTraits = decltype(__generic__impl__::callableTypeDeducer(std::declval<F>()));

template <typename F, typename = void>
struct callable_is_variadic : std::bool_constant<true> {};

template <typename F>
struct callable_is_variadic<F, std::void_t<CallableTraits<F>>> : std::bool_constant<false> {};

template <typename F>
constexpr const bool callable_is_variadic_v = callable_is_variadic<F>::value;

template <typename F>
constexpr size_t callable_argument_count(size_t expected)
{
    if constexpr (callable_is_variadic_v<F>) {
        return expected;
    } else {
        return CallableTraits<F>::argument_count;
    }
}

template <template <auto, typename, typename, typename...> typename C, auto f>
struct Partial {
    template <typename Arg1, typename Arg2, typename... Args>
    struct apply {
        using type = C<f, Arg1, Arg2, Args...>;
    };
};

template <auto f>
struct Callable {
    using traits = CallableTraits<decltype(f)>;
    static constexpr typename traits::type value = f;

    template <template <auto, typename, typename, typename...> typename C, typename... Args>
    struct instance {
        using type = typename traits::template instance<Partial<C, f>::template apply, Args...>::type;
    };
};

template <template <auto, typename, typename, typename...> typename C, auto f, typename... Args>
using MemberFunctionCapture = typename Callable<f>::template instance<C, Args...>::type;
}