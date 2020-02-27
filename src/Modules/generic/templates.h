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

template <typename>
struct type_holder_t {
};

template <typename T>
const constexpr type_holder_t<T> type_holder = {};

template <typename...>
struct type_pack;

template <size_t, typename T>
struct select_type;

template <size_t I, typename Head, typename... Tail>
struct select_type<I, type_pack<Head, Tail...>>
    : select_type<I - 1, type_pack<Tail...>> {
};

template <typename Head, typename... Tail>
struct select_type<0, type_pack<Head, Tail...>> {
    typedef Head type;
};

template <size_t I, typename T>
using select_type_t = typename select_type<I, T>::type;

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

template <size_t n, typename... _Ty>
using type_pack_n = typename __generic__impl__::type_pack_selector<type_pack<_Ty...>, n>::type;

template <typename Pack, typename T>
struct type_pack_contains : std::false_type {
};

template <typename... V, typename T>
struct type_pack_contains<type_pack<T, V...>, T> : std::true_type {
};

template <typename U, typename... V, typename T>
struct type_pack_contains<type_pack<U, V...>, T> : type_pack_contains<type_pack<V...>, T> {
};

template <typename Pack, typename T>
constexpr bool type_pack_contains_v = type_pack_contains<Pack, T>::value;

template <typename Pack, typename T>
struct type_pack_index {
};

template <typename T, typename U, typename... _Ty>
struct type_pack_index<type_pack<U, _Ty...>, T> : std::integral_constant<size_t, type_pack_index<type_pack<_Ty...>, T>::value + 1> {
};

template <typename T, typename... _Ty>
struct type_pack_index<type_pack<T, _Ty...>, T> : std::integral_constant<size_t, 0> {
};

template <typename Pack, typename T>
constexpr size_t type_pack_index_v = type_pack_index<Pack, T>::value;

template <bool...>
struct bool_pack;

template <bool... values>
struct all_of
    : std::is_same<bool_pack<values..., true>, bool_pack<true, values...>> {
};

template <typename V, typename T>
struct variant_contains;

template <typename T, typename... _Ty>
struct variant_contains<std::variant<_Ty...>, T> : type_pack_contains<type_pack<_Ty...>, T> {
};

template <typename V, typename T>
constexpr bool variant_contains_v = variant_contains<V, T>::value;

template <typename V, typename T>
struct variant_index;

template <typename T, typename... _Ty>
struct variant_index<std::variant<_Ty...>, T> : type_pack_index<type_pack<_Ty...>, T> {
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

template <typename, template <typename...> typename>
struct is_instance : public std::false_type {
};

template <typename... T, template <typename...> typename U>
struct is_instance<U<T...>, U> : public std::true_type {
};

template <typename T, template <typename...> typename U>
constexpr const bool is_instance_v = is_instance<T, U>::value;


struct DefaultTag;

template <typename Tag, size_t>
struct TaggedPlaceholder;

template <size_t N>
using Placeholder = TaggedPlaceholder<DefaultTag, N>;

template <typename T>
struct replace {
    template <typename Tag, typename...>
	using tagged = T;
	
	template <typename...>
    using type = T;	
};

template <typename Tag, size_t N> 
struct replace<TaggedPlaceholder<Tag, N>> {
	template <typename Tag2, typename... Args>
	using tagged = std::conditional_t<std::is_same_v<Tag, Tag2>, select_type_t<N, type_pack<Args...>>, TaggedPlaceholder<Tag,N>>;

    template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

template <template <typename...> typename C, typename... Ty>
struct replace<C<Ty...>> {
	template <typename Tag, typename... Args>
	using tagged = C<typename replace<Ty>::template tagged<Tag, Args...>...>;

	template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

template <template <auto, typename...> typename C, auto V, typename... Ty>
struct replace<C<V, Ty...>> {
    template <typename Tag, typename... Args>
    using tagged = C<V, typename replace<Ty>::template tagged<Tag, Args...>...>;

	template <typename... Args>
    using type = tagged<DefaultTag, Args...>;
};

}
