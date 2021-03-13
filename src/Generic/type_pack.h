#pragma once

namespace Engine {

template <typename...>
struct type_pack {
};

template <typename Pack>
struct type_pack_indices;

template <typename... T>
struct type_pack_indices<type_pack<T...>> {
    using type = std::make_index_sequence<sizeof...(T)>;
};

template <typename Pack>
using type_pack_indices_t = typename type_pack_indices<Pack>::type;

template <typename Pack>
struct type_pack_size;

template <typename... T>
struct type_pack_size<type_pack<T...>> {
    static constexpr const size_t value = sizeof...(T);
};

template <typename Pack>
constexpr const size_t type_pack_size_v = type_pack_size<Pack>::value;

template <size_t, typename T>
struct type_pack_select;

template <size_t I, typename Head, typename... Tail>
struct type_pack_select<I, type_pack<Head, Tail...>>
    : type_pack_select<I - 1, type_pack<Tail...>> {
};

template <typename Head, typename... Tail>
struct type_pack_select<0, type_pack<Head, Tail...>> {
    typedef Head type;
};

template <size_t I, typename T>
using type_pack_select_t = typename type_pack_select<I, T>::type;

template <typename T>
using type_pack_first = type_pack_select<0, T>;

template <typename T>
using type_pack_first_t = typename type_pack_first<T>::type;

template <typename Pack, typename Is>
struct type_pack_select_multiple;

template <typename Pack, size_t... Is>
struct type_pack_select_multiple<Pack, std::index_sequence<Is...>> {
    using type = type_pack<typename type_pack_select<Is, Pack>::type...>;
};

template <typename Pack, size_t n>
using type_pack_select_first_n = typename type_pack_select_multiple<Pack, std::make_index_sequence<n>>::type;

template <typename T>
using type_pack_pop_front = type_pack_select_multiple<T, make_index_range<1, type_pack_size_v<T>>>;

template <typename T>
using type_pack_pop_front_t = typename type_pack_pop_front<T>::type;

template <template <typename> typename F, typename Pack>
struct type_pack_apply;

template <template <typename> typename F, typename... Ty>
struct type_pack_apply<F, type_pack<Ty...>> {
    typedef type_pack<F<Ty>...> type;
};

template <template <typename> typename F, typename Pack>
using type_pack_apply_t = typename type_pack_apply<F, Pack>::type;

template <template <typename> typename F, typename Pack, size_t n, typename Is = type_pack_indices_t<Pack>>
struct type_pack_apply_to_nth;

template <template <typename> typename F, typename... Ty, size_t n, size_t... Is>
struct type_pack_apply_to_nth<F, type_pack<Ty...>, n, std::index_sequence<Is...>> {
    typedef type_pack<apply_if_t<F, Ty, n == Is>...> type;
};

template <template <typename> typename F, typename Pack, size_t n>
using type_pack_apply_to_nth_t = typename type_pack_apply_to_nth<F, Pack, n>::type;

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

template <typename IntType, typename T, typename RecPack, typename Pack, bool Selector = type_pack_contains_v<RecPack, T>>
struct type_pack_index_recurse;

template <typename IntType, typename T, typename RecPack, typename Pack>
struct type_pack_index_recurse<IntType, T, RecPack, Pack, true> : std::integral_constant<IntType, 0> {
};

template <typename IntType, typename T, typename RecPack, typename Pack>
struct type_pack_index_recurse<IntType, T, RecPack, Pack, false> : std::integral_constant<IntType, type_pack_index<IntType, Pack, T>::value + 1> {
};

template <typename IntType, typename T, typename... RecT, typename... _Ty>
struct type_pack_index<IntType, type_pack<type_pack<RecT...>, _Ty...>, T> : type_pack_index_recurse<IntType, T, type_pack<RecT...>, type_pack<_Ty...>> {
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

template <typename Pack>
struct type_pack_as_tuple;

template <typename... T>
struct type_pack_as_tuple<type_pack<T...>> {
    using type = std::tuple<T...>;
};

template <typename Pack>
using type_pack_as_tuple_t = typename type_pack_as_tuple<Pack>::type;

template <typename Pack, typename T>
struct type_pack_append;

template <typename T, typename... Ty>
struct type_pack_append<type_pack<Ty...>, T> {
    using type = type_pack<Ty..., T>;
};

template <typename Pack, typename T>
using type_pack_append_t = typename type_pack_append<Pack, T>::type;

template <typename Pack, typename... Filter>
struct type_pack_filter;

template <typename... Filter>
struct type_pack_filter<type_pack<>, Filter...> {
    using type = type_pack<>;
};

template <typename T, typename... Ty, typename... Filter>
struct type_pack_filter<type_pack<T, Ty...>, Filter...> {
private:
    using base = typename type_pack_filter<type_pack<Ty...>, Filter...>::type;

public:
    using type = std::conditional_t<
        type_pack_contains_v<type_pack<Filter...>, T>,
        base,
        type_pack_append_t<base, T>>;
};

template <typename Pack, typename... Filter>
using type_pack_filter_t = typename type_pack_filter<Pack, Filter...>::type;

template <typename Pack, template <typename> typename Filter>
struct type_pack_filter_if;

template <template <typename> typename Filter>
struct type_pack_filter_if<type_pack<>, Filter> {
    using type = type_pack<>;
};

template <typename T, typename... Ty, template <typename> typename Filter>
struct type_pack_filter_if<type_pack<T, Ty...>, Filter> {
private:
    using base = typename type_pack_filter_if<type_pack<Ty...>, Filter>::type;

public:
    using type = std::conditional_t<
        Filter<T>::value,
        type_pack_append_t<base, T>,
        base>;
};

template <typename Pack, template <typename> typename Filter>
using type_pack_filter_if_t = typename type_pack_filter_if<Pack, Filter>::type;

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
    using type = Default;
};

template <>
struct type_pack_unpack_unique<type_pack<>, void> {
};

template <typename Pack, typename Default = void>
using type_pack_unpack_unique_t = typename type_pack_unpack_unique<Pack, Default>::type;

}