#pragma once

namespace Engine {
template <typename T>
using identity = T;

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

template <typename...>
using void_t = void;

template <typename T, typename...>
using dependent_t = T;

template <typename T, bool b>
struct dependent_bool : std::bool_constant<b> {
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

namespace __generic_impl__ {
    template <template <typename...> typename, typename, typename...>
    struct can_apply : std::false_type {
    };
    template <template <typename...> typename Z, typename... Ts>
    struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...> : std::true_type {
    };
}
template <template <typename...> typename Z, typename... Ts>
using can_apply = __generic_impl__::can_apply<Z, void, Ts...>;

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

template <size_t add, typename Sequence>
struct index_range_add;

template <size_t add, size_t... Is>
struct index_range_add<add, std::index_sequence<Is...>> {
    using type = std::index_sequence<(Is + add)...>;
};

template <size_t from, size_t to>
using make_index_range = typename index_range_add<from, std::make_index_sequence<to - from>>::type;

template <typename Base, std::derived_from<Base> Derived>
constexpr size_t inheritance_offset()
{
    return reinterpret_cast<uintptr_t>(static_cast<Base *>(reinterpret_cast<Derived *>(0x1))) - 1;
}

template <typename T, template <typename...> typename U>
concept InstanceOf = is_instance<T, U>::value;

template <typename T>
concept Tuple = InstanceOf<T, std::tuple>;

template <typename T>
concept String = std::is_convertible_v<T &, const std::string &> && std::is_constructible_v<T, const std::string &>;

template <typename T>
concept Enum = std::is_enum_v<T>;

template <typename T>
concept Pointer = std::is_pointer_v<T> || std::is_null_pointer_v<T>;

template <typename T>
concept Function = std::is_function_v<T>;

}
