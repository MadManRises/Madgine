#pragma once

namespace Engine {

template <typename Sequence, template <size_t...> typename Template>
struct index_sequence_instantiate;

template <size_t... Is, template <size_t...> typename Template>
struct index_sequence_instantiate<std::index_sequence<Is...>, Template> {
    using type = Template<Is...>;
};

template <typename Sequence, template <size_t...> typename Template>
using index_sequence_instantiate_t = typename index_sequence_instantiate<Sequence, Template>::type;

template <typename...>
struct type_pack;

template <>
struct type_pack<> {

    template <typename T>
    using append = type_pack<T>;
    template <typename T>
    using prepend = type_pack<T>;
    template <bool Cond, typename T>
    using prepend_if = std::conditional_t<Cond, type_pack<T>, type_pack<>>;

    template <template <typename> typename Filter>
    using filter = type_pack<>;

    template <template <typename...> typename Wrapper>
    using instantiate = Wrapper<>;

    using as_tuple = instantiate<std::tuple>;
};

template <typename Head, typename... Ty>
struct type_pack<Head, Ty...> {

    template <size_t I>
    struct recurse : type_pack<Ty...>::template recurse<I - 1> {
        using front = typename type_pack<Ty...>::template recurse<I - 1>::front::template prepend<Head>;
    };

    template <>
    struct recurse<0> {
        using front = type_pack<>;
        using type = Head;
        using tail = type_pack<Ty...>;
    };

    using first = Head;

    using indices = std::index_sequence_for<Head, Ty...>;
    static constexpr const size_t size = 1 + sizeof...(Ty);

    template <template <typename> typename F>
    using transform = type_pack<F<Head>, F<Ty>...>;
    template <template <typename> typename F, size_t n>
    using transform_nth = typename recurse<n>::front::template append<F<typename recurse<n>::type>>::template concat<typename recurse<n>::tail>;

    template <template <typename> typename Filter>
    using filter = typename type_pack<Ty...>::template filter<Filter>::prepend_if<Filter<Head>::value, Head>;

    using pop_front = type_pack<Ty...>;
    template <typename T>
    using append = type_pack<Head, Ty..., T>;
    template <typename T>
    using prepend = type_pack<T, Head, Ty...>;
    template <bool Cond, typename T>
    using prepend_if = std::conditional_t<Cond, type_pack<T, Head, Ty...>, type_pack<Head, Ty...>>;
    
    template <template <typename...> typename Wrapper>
    using instantiate = Wrapper<Head, Ty...>;

    using as_tuple = instantiate<std::tuple>;

    template <size_t I>
    using select = typename recurse<I>::type;
    template <size_t... Is>
    using select_multiple = type_pack<select<Is>...>;
    template <size_t n>
    using select_first_n = index_sequence_instantiate_t<std::make_index_sequence<n>, select_multiple>;
};

template <typename Pack>
using type_pack_first = typename Pack::first;

template <typename Pack>
using type_pack_as_tuple = typename Pack::as_tuple;

template <typename T>
struct type_pack_appender {
    template <typename Pack>
    using type = typename Pack::template append<T>;
};


template <typename Pack, typename T>
struct type_pack_contains : std::false_type {
};

template <typename... V, typename T>
struct type_pack_contains<type_pack<T, V...>, T> : std::true_type {
};

template <typename U, typename... V, typename T>
struct type_pack_contains<type_pack<U, V...>, T> : type_pack_contains<type_pack<V...>, T> {
};

template <typename... U, typename... V, typename T>
struct type_pack_contains<type_pack<type_pack<U...>, V...>, T> : type_pack_contains<type_pack<U..., V...>, T> {
};

template <typename Pack, typename T>
constexpr bool type_pack_contains_v = type_pack_contains<Pack, T>::value;

template <typename IntType, typename Pack, typename T>
struct type_pack_index {
};

template <typename IntType, typename T, typename U, typename... _Ty>
struct type_pack_index<IntType, type_pack<U, _Ty...>, T> : std::integral_constant<IntType, type_pack_index<IntType, type_pack<_Ty...>, T>::value + 1> {
};

template <typename IntType, typename T, typename... _Ty>
struct type_pack_index<IntType, type_pack<T, _Ty...>, T> : std::integral_constant<IntType, 0> {
};

namespace __generic_impl__ {

    template <typename IntType, typename T, typename RecPack, typename Pack, bool Selector = type_pack_contains_v<RecPack, T>>
    struct type_pack_index_recurse;

    template <typename IntType, typename T, typename RecPack, typename Pack>
    struct type_pack_index_recurse<IntType, T, RecPack, Pack, true> : std::integral_constant<IntType, 0> {
    };

    template <typename IntType, typename T, typename RecPack, typename Pack>
    struct type_pack_index_recurse<IntType, T, RecPack, Pack, false> : std::integral_constant<IntType, type_pack_index<IntType, Pack, T>::value + 1> {
    };

}

template <typename IntType, typename T, typename... RecT, typename... _Ty>
struct type_pack_index<IntType, type_pack<type_pack<RecT...>, _Ty...>, T> : __generic_impl__::type_pack_index_recurse<IntType, T, type_pack<RecT...>, type_pack<_Ty...>> {
};

template <typename IntType, typename Pack, typename T>
constexpr IntType type_pack_index_v = type_pack_index<IntType, Pack, T>::value;

namespace __generic_impl__ {
    template <typename T, size_t I>
    using lift_by_index = T;

    template <typename T, typename Is>
    struct type_pack_repeat_n_times_helper;

    template <typename T, size_t... Is>
    struct type_pack_repeat_n_times_helper<T, std::index_sequence<Is...>> {
        using type = type_pack<lift_by_index<T, Is>...>;
    };
}

template <typename T, size_t n>
using type_pack_repeat_n_times = typename __generic_impl__::type_pack_repeat_n_times_helper<T, std::make_index_sequence<n>>::type;


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
struct variant_index<std::variant<_Ty...>, T> : type_pack_index<size_t, type_pack<_Ty...>, T> {
};

template <typename Pack, typename Default = void>
struct type_pack_unpack_unique {
    static_assert(dependent_bool<Pack, false>::value, "type_pack containing 2 elements passed to type_pack_unpack_unique");
};

template <typename Unique, typename Default>
struct type_pack_unpack_unique<type_pack<Unique>, Default> {
    using type = Unique;
};

template <typename Default>
struct type_pack_unpack_unique<type_pack<>, Default> {
    static_assert(!std::same_as<Default, void>, "empty type_pack passed to type_pack_unpack_unique without default");
    using type = Default;
};

template <typename Pack, typename Default = void>
using type_pack_unpack_unique_t = typename type_pack_unpack_unique<Pack, Default>::type;

}