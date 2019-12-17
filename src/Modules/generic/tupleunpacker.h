#pragma once

#include "callable_traits.h"

namespace Engine {
namespace TupleUnpacker {

#define LIFT(fname)                                            \
    [&](auto &&... args) -> decltype(auto) {                   \
        return (fname)(std::forward<decltype(args)>(args)...); \
    }

#define LIFT_MEMBER(fname)                                                                           \
    [&](auto &&_this, auto &&... args) -> decltype(auto) {                                           \
        return (std::forward<decltype(_this)>(_this)->fname)(std::forward<decltype(args)>(args)...); \
    }

    template <size_t I, typename Tuple, size_t... S, size_t... T>
    decltype(auto) expand(Tuple &&tuple, std::index_sequence<S...>, std::index_sequence<T...>)
    {
        return std::tuple_cat(
            std::forward_as_tuple(std::get<S>(std::forward<Tuple>(tuple))...),
            std::get<I>(std::forward<Tuple>(tuple)),
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

    template <class F, size_t... S, class Tuple>
    decltype(auto) unpackTuple(F &&f, Tuple &&args, std::index_sequence<S...>)
    {
        return std::invoke(std::forward<F>(f), std::get<S>(std::forward<Tuple>(args))...);
    }

    template <class F, class Tuple>
    decltype(auto) invokeFromTuple(F &&f, Tuple &&args)
    {
        return unpackTuple(std::forward<F>(f), std::forward<Tuple>(args),
            std::make_index_sequence<callable_argument_count<F>(std::tuple_size<std::remove_reference_t<Tuple>>::value)>());
    }

    template <typename F, typename... Args>
    decltype(auto) invoke(F &&f, Args &&... args)
    {
        return invokeFromTuple(std::forward<F>(f), std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename F, typename... Args>
    decltype(auto) invokeExpand(F &&f, Args &&... args)
    {
        return invokeFromTuple(std::forward<F>(f), expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    template <typename R, typename F, typename... Args>
    std::enable_if_t<std::is_convertible_v<typename CallableTraits<F>::return_type, R>, R> invokeDefaultResult(R &&, F &&f, Args &&... args)
    {
        return invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename R, typename F, typename... Args>
    std::enable_if_t<!std::is_convertible_v<typename CallableTraits<F>::return_type, R>, R> invokeDefaultResult(R &&defaultValue, F &&f, Args &&... args)
    {
        TupleUnpacker::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        return std::forward<R>(defaultValue);
    }

    template <typename T, size_t... S, class Tuple>
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
    T construct(Args &&... args)
    {
        return constructFromTuple<T>(std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    T constructExpand(Args &&... args)
    {
        return constructFromTuple<T>(expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

};

}