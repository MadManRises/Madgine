#pragma once

#include "Generic/defaultassign.h"

namespace Engine {
namespace TupleUnpacker {

#define LIFT(fname, ...)                                       \
    [__VA_ARGS__](auto &&... args) -> decltype(auto) {         \
        return (fname)(std::forward<decltype(args)>(args)...); \
    }

#define LIFT_MEMBER(fname, ...)                                                                      \
    [__VA_ARGS__](auto &&_this, auto &&... args) -> decltype(auto) {                                 \
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
    R invokeDefaultResult(R &&defaultValue, F &&f, Args &&... args)
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
    T construct(Args &&... args)
    {
        return constructFromTuple<T>(std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    T constructExpand(Args &&... args)
    {
        return constructFromTuple<T>(expand<sizeof...(args) - 1>(std::forward_as_tuple(std::forward<Args>(args)...)));
    }

    struct ubiq_constructor {
        template <typename T>
        constexpr operator T() const noexcept;
    };

    template <typename T, size_t I0, size_t... Is>
    constexpr dependent_t<size_t, decltype(T { ubiq_constructor {}, (Is, ubiq_constructor {})... })>
        detect_fields_count(std::index_sequence<I0, Is...>)
    {
        return sizeof...(Is) + 1;
    }

    template <typename T, size_t... Is>
    constexpr size_t detect_fields_count(std::index_sequence<Is...>)
    {
        if constexpr (sizeof...(Is) > 0)
            return detect_fields_count<T>(std::make_index_sequence<sizeof...(Is) - 1>());
        else
            return 0;
    }

    template <typename... _Ty>
    constexpr std::tuple<_Ty...> returnAsTuple(_Ty &&... args) noexcept
    { // forward arguments in a tuple
        return std::tuple<_Ty...> { std::forward<_Ty>(args)... };
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 1>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a] = std::forward<T>(t);

            return returnAsTuple(a);
        } else {
            auto &&[a] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 2>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b] = std::forward<T>(t);

            return returnAsTuple(a, b);
        } else {
            auto &&[a, b] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 3>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c] = std::forward<T>(t);

            return returnAsTuple(a, b, c);
        } else {
            auto &&[a, b, c] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 4>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d);
        } else {
            auto &&[a, b, c, d] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 5>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d, e] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d, e);
        } else {
            auto &&[a, b, c, d, e] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d), std::forward<decltype(e)>(e));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 6>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d, e, f] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d, e, f);
        } else {
            auto &&[a, b, c, d, e, f] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d), std::forward<decltype(e)>(e), std::forward<decltype(f)>(f));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 7>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d, e, f, g] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d, e, f, g);
        } else {
            auto &&[a, b, c, d, e, f, g] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d), std::forward<decltype(e)>(e), std::forward<decltype(f)>(f), std::forward<decltype(g)>(g));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 8>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d, e, f, g, h] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d, e, f, g, h);
        } else {
            auto &&[a, b, c, d, e, f, g, h] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d), std::forward<decltype(e)>(e), std::forward<decltype(f)>(f), std::forward<decltype(g)>(g), std::forward<decltype(h)>(h));
        }
    }

    template <typename T>
    auto toTupleImpl(T &&t, std::integral_constant<size_t, 9>)
    {
        if constexpr (std::is_reference_v<T>) {
            auto &[a, b, c, d, e, f, g, h, i] = std::forward<T>(t);

            return returnAsTuple(a, b, c, d, e, f, g, h, i);
        } else {
            auto &&[a, b, c, d, e, f, g, h, i] = std::forward<T>(t);

            return returnAsTuple(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::forward<decltype(c)>(c), std::forward<decltype(d)>(d), std::forward<decltype(e)>(e), std::forward<decltype(f)>(f), std::forward<decltype(g)>(g), std::forward<decltype(h)>(h), std::forward<decltype(i)>(i));
        }
    }

    //TODO: Use only is_standard_layout as soon as UnitHelper is cleaned up
    template <typename T>
    struct is_tuplefyable : std::bool_constant<std::is_standard_layout_v<T> && std::is_trivially_copy_constructible_v<T> && !std::is_fundamental_v<T> && !std::is_pointer_v<T> && !std::is_enum_v<T>> {
        static constexpr size_t elementCount = detect_fields_count<std::remove_reference_t<T>>(std::make_index_sequence<sizeof(T)>());
    };

    template <typename T>
    auto toTuple(T &&t)
    {
        return toTupleImpl(std::forward<T>(t), std::integral_constant<size_t, is_tuplefyable<std::remove_const_t<std::remove_reference_t<T>>>::elementCount> {});
    }

    template <typename T>
    constexpr const bool is_tuplefyable_v = is_tuplefyable<std::remove_const_t<std::remove_reference_t<T>>>::value;

#define MAKE_TUPLEFYABLE(Type, Count)                                       \
    template <>                                                             \
    struct ::Engine::TupleUnpacker::is_tuplefyable<Type> : std::true_type { \
        static constexpr size_t elementCount = Count;                       \
    };

#define MAKE_NOT_TUPLEFYABLE(Type, ...)                                           \
    template <__VA_ARGS__>                                                              \
    struct ::Engine::TupleUnpacker::is_tuplefyable<Type> : std::false_type { \
    };

    template <typename Tuple, typename F, size_t... Is>
    auto forEach(Tuple &&t, F &&f, std::index_sequence<Is...>)
    {
        if constexpr (((!std::is_same_v<std::invoke_result_t<F, decltype(std::get<Is>(std::forward<Tuple>(t)))>, void>)&&...))
            return std::tuple<std::invoke_result_t<F, decltype(std::get<Is>(std::forward<Tuple>(t)))>...> { f(std::get<Is>(std::forward<Tuple>(t)))... };
        else
            (f(std::get<Is>(std::forward<Tuple>(t))), ...);
    }

    template <typename Tuple, typename F>
    auto forEach(Tuple &&t, F &&f)
    {
        return forEach(std::forward<Tuple>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
    }

    template <typename Tuple, typename F, typename T, size_t... Is>
    T accumulate(Tuple &&tuple, F &&f, T &&t, std::index_sequence<Is...>)
    {
        DefaultAssign assign;
        (assign(t, f(std::get<Is>(std::forward<Tuple>(tuple)), std::forward<T>(t))), ...);
        return std::forward<T>(t);
    }

    template <typename Tuple, typename F, typename T>
    T accumulate(Tuple &&tuple, F &&f, T &&t)
    {
        return accumulate(std::forward<Tuple>(tuple), std::forward<F>(f), std::forward<T>(t), std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
    }

    template <typename Tuple, typename F, size_t... Is>
    decltype(auto) select(Tuple &&t, F &&f, size_t index, std::index_sequence<Is...>)
    {
        /*if constexpr (((!std::is_same_v<std::invoke_result_t<F, decltype(std::get<Is>(std::forward<Tuple>(t)))>, void>)&&...))
            return std::tuple<std::invoke_result_t<F, decltype(std::get<Is>(std::forward<Tuple>(t)))>...> { f(std::get<Is>(std::forward<Tuple>(t)))... };
        else
            (f(std::get<Is>(std::forward<Tuple>(t))), ...);*/
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

};

}