#pragma once

namespace Engine {
template <typename T>
using id = T;

template <typename V, typename...>
struct last {
    typedef V type;
};

template <typename U, typename V, typename... T>
struct last<U, V, T...> : public last<V, T...> {
};

template <typename V, typename...>
struct first {
    typedef V type;
};

template <typename...>
using void_t = void;

template <typename T, typename...>
using dependent_t = T;

template <auto>
struct auto_holder_t {
};

template <auto A>
const constexpr auto_holder_t<A> auto_holder = {};

template <bool...>
struct bool_pack;

template <bool... values>
struct all_of
    : std::is_same<bool_pack<values..., true>, bool_pack<true, values...>> {
};

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...)->overloaded<Ts...>;

template <typename T, size_t S>
constexpr size_t array_size(T (&)[S])
{
    return S;
}

template <typename T, bool b>
struct dependent_bool : std::bool_constant<b> {
};

namespace details {
    template <template <typename...> typename, typename, typename...>
    struct can_apply : std::false_type {
    };
    template <template <typename...> typename Z, typename... Ts>
    struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...> : std::true_type {
    };
}
template <template <typename...> typename Z, typename... Ts>
using can_apply = details::can_apply<Z, void, Ts...>;

template <template <typename> typename F, typename T, bool b>
struct apply_if {
    using type = T;
};

template <template <typename> typename F, typename T>
struct apply_if<F, T, true> {
    using type = F<T>;
};

template <template <typename> typename F, typename T, bool b>
using apply_if_t = typename apply_if<F, T, b>::type;

template <typename, template <typename...> typename>
struct is_instance : public std::false_type {
};

template <typename... T, template <typename...> typename U>
struct is_instance<U<T...>, U> : public std::true_type {
};

template <typename T, template <typename...> typename U>
constexpr const bool is_instance_v = is_instance<T, U>::value;

template <typename T>
struct is_tuple : std::false_type {
};

template <typename... Ty>
struct is_tuple<std::tuple<Ty...>> : std::true_type {
};

template <typename T>
constexpr const bool is_tuple_v = is_tuple<T>::value;

}
