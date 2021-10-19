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

template <typename... Args>
using first_t = typename first<Args...>::type;


template <template <typename> typename Cond>
struct negate {
    template <typename T>
    struct type {
        static constexpr bool value = !Cond<T>::value;
    };
    template <typename T>
    static constexpr bool type_v = type<T>::value;
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
overloaded(Ts...) -> overloaded<Ts...>;

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
struct is_instance : std::false_type {
};

template <typename... T, template <typename...> typename U>
struct is_instance<U<T...>, U> : std::true_type {
    using argument_types = type_pack<T...>;
};

template <typename T, template <typename...> typename U>
constexpr const bool is_instance_v = is_instance<T, U>::value;

template <typename T, template <typename> typename U>
concept instance_of = is_instance_v<T, U>;

template <typename T>
using is_tuple = is_instance<T, std::tuple>;

template <typename T>
constexpr const bool is_tuple_v = is_tuple<T>::value;

template <typename T>
using is_string_like = std::bool_constant<std::is_convertible_v<T &, const std::string &> && std::is_constructible_v<T, const std::string &>>;

template <typename T>
constexpr const bool is_string_like_v = is_string_like<T>::value;

template <size_t add, typename Sequence>
struct index_range_add;

template <size_t add, size_t... Is>
struct index_range_add<add, std::index_sequence<Is...>> {
    using type = std::index_sequence<(Is + add)...>;
};

template <size_t from, size_t to>
using make_index_range = typename index_range_add<from, std::make_index_sequence<to - from>>::type;

template <typename Base, typename Derived>
constexpr size_t inheritance_offset() {
    static_assert(std::is_base_of_v<Base, Derived>);
    return reinterpret_cast<uintptr_t>(static_cast<Base *>(reinterpret_cast<Derived *>(0x1))) - 1;
}

template <typename T>
concept Enum = std::is_enum_v<T>;

}
