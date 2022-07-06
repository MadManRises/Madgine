#pragma once

namespace Engine {

template <typename Sequence, template <size_t...> typename T>
struct index_sequence_instantiate;

template <size_t... Is, template <size_t...> typename T>
struct index_sequence_instantiate<std::index_sequence<Is...>, T> {
    using type = T<Is...>;
};

template <typename Sequence, template <size_t...> typename T>
using index_sequence_instantiate_t = typename index_sequence_instantiate<Sequence, T>::type;

template <typename...>
struct type_pack;

template <>
struct type_pack<> {

    template <typename T>
    using append = type_pack<T>;
    template <typename... T>
    using prepend = type_pack<T...>;
    template <bool Cond, typename T>
    using prepend_if = std::conditional_t<Cond, type_pack<T>, type_pack<>>;

    template <template <typename> typename Filter>
    using filter = type_pack<>;

    template <template <typename...> typename Wrapper>
    using instantiate = Wrapper<>;

    using as_tuple = instantiate<std::tuple>;

    template <typename T>
    using unique = T;

    template <typename Default>
    using unpack_unique = Default;
};

template <typename Head, typename... Ty>
struct type_pack<Head, Ty...> {

    struct helpers {
        template <size_t I>
        struct recurse : type_pack<Ty...>::helpers::template recurse<I - 1> {
        };

        template <>
        struct recurse<0> {
            using type = Head;
        };

        template <typename, typename T>
        struct is_or_contains : std::false_type {
            template <typename IntType, typename... Tail>
            static constexpr IntType index = 1 + type_pack<Tail...>::template index<IntType, T>;
        };

        template <typename T>
        struct is_or_contains<T, T> : std::true_type {
            template <typename IntType, typename... Tail>
            static constexpr IntType index = 0;
        };

        template <typename... Ty2, typename T>
        requires type_pack<Ty2...>::template contains<T> 
        struct is_or_contains<type_pack<Ty2...>, T> : is_or_contains<T, T> {
        };
    };

    using first = Head;

    using indices = std::index_sequence_for<Head, Ty...>;
    static constexpr const size_t size = 1 + sizeof...(Ty);

    template <template <typename> typename F>
    using transform = type_pack<F<Head>, F<Ty>...>;

    template <template <typename> typename Filter>
    using filter = typename type_pack<Ty...>::template filter<Filter>::template prepend_if<Filter<Head>::value, Head>;

    using pop_front = type_pack<Ty...>;
    template <typename T>
    using append = type_pack<Head, Ty..., T>;
    template <typename... T>
    using prepend = type_pack<T..., Head, Ty...>;
    template <bool Cond, typename T>
    using prepend_if = std::conditional_t<Cond, type_pack<T, Head, Ty...>, type_pack<Head, Ty...>>;

    template <typename Pack2>
    using concat = typename Pack2::template prepend<Head, Ty...>;

    template <template <typename...> typename Wrapper>
    using instantiate = Wrapper<Head, Ty...>;

    using as_tuple = instantiate<std::tuple>;

    template <size_t I>
    using select = typename helpers::template recurse<I>::type;
    template <size_t... Is>
    using select_multiple = type_pack<select<Is>...>;
    template <size_t n>
    using select_first_n = index_sequence_instantiate_t<std::make_index_sequence<n>, select_multiple>;

    template <typename T>
    static constexpr bool contains = (helpers::template is_or_contains<Head, T>::value || ... || helpers::template is_or_contains<Ty, T>::value);

    template <typename IntType, typename T>
    static constexpr IntType index = helpers::template is_or_contains<Head, T>::template index<IntType, Ty...>;

    template <typename T>
    struct unique {
        static_assert(dependent_bool<T, false>::value, "unpack_unique passed to type_pack containing 2 or more elements");
    };

    template <typename Default = void>
    using unpack_unique = typename type_pack<Ty...>::template unique<Head>;
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

}