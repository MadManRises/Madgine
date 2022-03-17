#pragma once

#include "Generic/defaultassign.h"

namespace Engine {
namespace TupleUnpacker {

#define LIFT(fname, ...)                                       \
    [__VA_ARGS__](auto &&...args) -> decltype(auto) {          \
        return (fname)(std::forward<decltype(args)>(args)...); \
    }

    template <typename Tuple>
    Tuple &&shiftTupleReference(Tuple &&tuple)
    {
        return std::forward<Tuple>(tuple);
    }

    template <typename... T, size_t... Is>
    std::tuple<T &...> shiftTupleReference(std::tuple<T...> &tuple, std::index_sequence<Is...>)
    {
        return { std::get<Is>(tuple)... };
    }

    template <typename... T>
    std::tuple<T &...> shiftTupleReference(std::tuple<T...> &tuple)
    {
        return shiftTupleReference(tuple, std::index_sequence_for<T...> {});
    }

    template <size_t I, typename Tuple, size_t... S, size_t... T>
    decltype(auto) expand(Tuple &&tuple, std::index_sequence<S...>, std::index_sequence<T...>)
    {
        return std::tuple_cat(
            std::forward_as_tuple(std::get<S>(std::forward<Tuple>(tuple))...),
            shiftTupleReference(std::get<I>(std::forward<Tuple>(tuple))),
            std::forward_as_tuple(std::get<I + 1 + T>(std::forward<Tuple>(tuple))...));
    }

    template <size_t I, typename Tuple>
    decltype(auto) expand(Tuple &&tuple)
    {
        constexpr size_t S = std::tuple_size_v<Tuple>;
        return expand<I>(
            std::forward<Tuple>(tuple),
            std::make_index_sequence<I>(),
            std::make_index_sequence<S - 1 - I>());
    }

    template <typename Tuple, size_t... Is>
    decltype(auto) flatten(Tuple &&tuple, std::index_sequence<Is...>);

    template <typename NonTuple>
    decltype(auto) flatten(NonTuple &&t)
    {
        return std::forward_as_tuple(std::forward<NonTuple>(t));
    }

    template <Tuple Tuple>
    decltype(auto) flatten(Tuple &&tuple)
    {
        return flatten(std::forward<Tuple>(tuple), std::make_index_sequence<std::tuple_size_v<Tuple>>());
    }

    template <typename Tuple, size_t... Is>
    decltype(auto) flatten(Tuple &&tuple, std::index_sequence<Is...>)
    {
        return std::tuple_cat(flatten(std::get<Is>(std::forward<Tuple>(tuple)))...);
    }

    template <typename F, size_t... S, typename Tuple>
    decltype(auto) unpackTuple(F &&f, Tuple &&args, std::index_sequence<S...>)
    {
        return std::invoke(std::forward<F>(f), std::get<S>(std::forward<Tuple>(args))...);
    }

    template <typename F, typename Tuple>
    decltype(auto) invokeFromTuple(F &&f, Tuple &&args)
    {
        return unpackTuple(std::forward<F>(f), std::forward<Tuple>(args),
            std::make_index_sequence<callable_argument_count<F>(std::tuple_size<std::remove_reference_t<Tuple>>::value)>());
    }

    template <typename F, typename... Args>
    decltype(auto) invoke(F &&f, Args &&...args)
    {
        return invokeFromTuple(std::forward<F>(f), std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename F, typename... Args>
    decltype(auto) invokeExpand(F &&f, Args &&...args)
    {
        return invokeFromTuple(std::forward<F>(f), expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    template <typename F, typename... Args>
    decltype(auto) invokeFlatten(F &&f, Args &&...args)
    {
        return invokeFromTuple(std::forward<F>(f), flatten(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    template <typename R, typename F, typename... Args>
    R invokeDefaultResult(R &&defaultValue, F &&f, Args &&...args)
    {
        using result_t = decltype(TupleUnpacker::invoke(std::forward<F>(f), std::forward<Args>(args)...));
        if constexpr (std::is_convertible_v<result_t, R>) {
            return TupleUnpacker::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        } else {
            TupleUnpacker::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            return std::forward<R>(defaultValue);
        }
    }

    template <typename T, size_t... S, typename Tuple>
    T constructUnpackTuple(Tuple &&args, std::index_sequence<S...>)
    {
        return T { std::get<S>(std::forward<Tuple>(args))... };
    }

    template <typename T, typename Tuple>
    T constructFromTuple(Tuple &&args)
    {
        return constructUnpackTuple<T>(std::forward<Tuple>(args), std::make_index_sequence<std::tuple_size_v<Tuple>>());
    }

    template <typename T, typename... Args>
    T construct(Args &&...args)
    {
        return constructFromTuple<T>(std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    T constructExpand(Args &&...args)
    {
        return constructFromTuple<T>(expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    template <typename T>
    auto num_bindings_impl() noexcept
    {
        return overloaded {
            [](auto &&u, unsigned) -> std::integral_constant<size_t, 0> { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a] = u; std::integral_constant<size_t, 1>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b] = u; std::integral_constant<size_t, 2>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c] = u; std::integral_constant<size_t, 3>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d] = u; std::integral_constant<size_t, 4>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d,e] = u; std::integral_constant<size_t, 5>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d,e,f] = u; std::integral_constant<size_t, 6>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d,e,f,g] = u; std::integral_constant<size_t, 7>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d,e,f,g,h] = u; std::integral_constant<size_t, 8>{}; })) { return {}; },
            [](auto &&u, int) -> decltype(({auto&& [a,b,c,d,e,f,g,h,i] = u; std::integral_constant<size_t, 9>{}; })) { return {}; }
        }(std::declval<T>(), int {});
    }

    template <typename T, size_t size>
    concept TuplefyableHelper = decltype(num_bindings_impl<T>()) {} == size;

    auto toTuple(TuplefyableHelper<1> auto &t)
    {
        auto &[a] = t;

        return std::tie(a);
    }

    auto toTuple(TuplefyableHelper<2> auto &t)
    {
        auto &[a, b] = t;

        return std::tie(a, b);
    }

    auto toTuple(TuplefyableHelper<3> auto &t)
    {
        auto &[a, b, c] = t;

        return std::tie(a, b, c);
    }

    auto toTuple(TuplefyableHelper<4> auto &t)
    {
        auto &[a, b, c, d] = t;

        return std::tie(a, b, c, d);
    }

    auto toTuple(TuplefyableHelper<5> auto &t)
    {
        auto &[a, b, c, d, e] = t;

        return std::tie(a, b, c, d, e);
    }

    auto toTuple(TuplefyableHelper<6> auto &t)
    {
        auto &[a, b, c, d, e, f] = t;

        return std::tie(a, b, c, d, e, f);
    }

    auto toTuple(TuplefyableHelper<7> auto &t)
    {
        auto &[a, b, c, d, e, f, g] = t;

        return std::tie(a, b, c, d, e, f, g);
    }

    auto toTuple(TuplefyableHelper<8> auto &t)
    {
        auto &[a, b, c, d, e, f, g, h] = t;

        return std::tie(a, b, c, d, e, f, g, h);
    }

    auto toTuple(TuplefyableHelper<9> auto &t)
    {
        auto &[a, b, c, d, e, f, g, h, i] = t;

        return std::tie(a, b, c, d, e, f, g, h, i);
    }

    template <typename T>
    concept Tuplefyable = decltype(num_bindings_impl<T>()) {} > 0;

    template <typename Tuple, typename F, size_t... Is>
    auto forEach(Tuple &&t, F &&f, std::index_sequence<Is...>)
    {
        return std::tuple { invoke_patch_void(std::forward<F>(f), std::get<Is>(std::forward<Tuple>(t)))... };
    }

    template <typename Tuple, typename F>
    auto forEach(Tuple &&t, F &&f)
    {
        return forEach(std::forward<Tuple>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
    }

    template <typename Tuple, typename F, typename T>
    T accumulate(Tuple &&tuple, F &&f, T &&t)
    {
        forEach(std::forward<Tuple>(tuple), [&](auto &&e) {
            t = f(std::forward<decltype(e)>(e), std::forward<T>(t));
        });
        return std::forward<T>(t);
    }

    template <typename Tuple, typename F, size_t... Is>
    decltype(auto) select(Tuple &&t, F &&f, size_t index, std::index_sequence<Is...>)
    {
        using R = std::invoke_result_t<F, decltype(std::get<0>(std::forward<Tuple>(t)))>;
        using Fs = R (*)(Tuple &&, F &&);
        constexpr Fs fs[] = {
            [](Tuple &&t, F &&f) -> R {
                return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t)));
            }...
        };
        return fs[index](std::forward<Tuple>(t), std::forward<F>(f));
    }

    template <typename Tuple, typename F>
    decltype(auto) select(Tuple &&t, F &&f, size_t index)
    {
        return select(std::forward<Tuple>(t), std::forward<F>(f), index, std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
    }
}
}