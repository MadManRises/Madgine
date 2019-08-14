#pragma once

namespace Engine {
template <class V, class...>
struct last {
    typedef V type;
};

template <class U, class V, class... T>
struct last<U, V, T...> : public last<V, T...> {
};

template <class...>
using void_t = void;

template <class...>
struct type_pack;

template <size_t, class T>
struct select_type;

template <size_t I, class Head, class... Tail>
struct select_type<I, type_pack<Head, Tail...>>
    : select_type<I - 1, type_pack<Tail...>> {
};

template <class Head, class... Tail>
struct select_type<0, type_pack<Head, Tail...>> {
    typedef Head type;
};

namespace __generic__impl__ {
    template <typename T, size_t n, size_t... Is>
    struct type_pack_selector
        : type_pack_selector<T, n - 1, n - 1, Is...> {
    };

    template <typename T, size_t... Is>
    struct type_pack_selector<T, 0, Is...> {
        using type = type_pack<typename select_type<Is, T>::type...>;
    };
}

template <size_t n, class... _Ty>
using type_pack_n = typename __generic__impl__::type_pack_selector<type_pack<_Ty...>, n>::type;

template <class Pack, class T>
struct type_pack_contains : std::false_type {
};

template <class... V, class T>
struct type_pack_contains<type_pack<T, V...>, T> : std::true_type {
};

template <class U, class... V, class T>
struct type_pack_contains<type_pack<U, V...>, T> : type_pack_contains<type_pack<V...>, T> {
};

template <class Pack, class T>
constexpr bool type_pack_contains_v = type_pack_contains<Pack, T>::value;

template <class Pack, class T>
struct type_pack_index {
};

template <class T, class U, class... _Ty>
struct type_pack_index<type_pack<U, _Ty...>, T> : std::integral_constant<size_t, type_pack_index<type_pack<_Ty...>, T>::value + 1> {
};

template <class T, class... _Ty>
struct type_pack_index<type_pack<T, _Ty...>, T> : std::integral_constant<size_t, 0> {
};

template <class Pack, class T>
constexpr size_t type_pack_index_v = type_pack_index<Pack, T>::value;

template <bool...>
struct bool_pack;

template <bool... values>
struct all_of
    : std::is_same<bool_pack<values..., true>, bool_pack<true, values...>> {
};

template <class V, class T>
struct variant_contains;

template <class T, class... _Ty>
struct variant_contains<std::variant<_Ty...>, T> : type_pack_contains<type_pack<_Ty...>, T> {
};

template <class V, class T>
struct variant_index;

template <class T, class... _Ty>
struct variant_index<std::variant<_Ty...>, T> : type_pack_index<type_pack<_Ty...>, T> {
};

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

template <class T, size_t S>
constexpr size_t array_size(T (&)[S])
{
    return S;
}

template <typename T, bool b>
struct dependent_bool : std::bool_constant<b> {
};

namespace details {
    template <template <class...> class, class, class...>
    struct can_apply : std::false_type {
    };
    template <template <class...> class Z, class... Ts>
    struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...> : std::true_type {
    };
}
template <template <class...> class Z, class... Ts>
using can_apply = details::can_apply<Z, void, Ts...>;

}
