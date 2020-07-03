#pragma once

namespace Engine {

namespace Derive_impl {
    template <int I>
    struct prio {
    };

    template <int I>
    struct upToPrio : upToPrio<I - 1> {
    };

    template <>
    struct upToPrio<0> {
    };

    struct Dummy {
        template <typename T>
        operator T();
    };
}

#define DERIVE_FRIEND(Name, ...) \
    template <typename...>       \
    friend struct __VA_ARGS__ Derive_impl_##Name##_def;

#define DERIVE_FUNCTION(Name, ...)                                                                                                                                                               \
    template <typename... Args>                                                                                                                                                                  \
    struct Derive_impl_##Name##_def {                                                                                                                                                            \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::prio<0>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()...));                                                                   \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::upToPrio<0>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()...));                                                               \
                                                                                                                                                                                                 \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::prio<1>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()..., Engine::Derive_impl::Dummy {}));                                    \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::upToPrio<1>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()..., Engine::Derive_impl::Dummy {}));                                \
                                                                                                                                                                                                 \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::prio<2>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()..., Engine::Derive_impl::Dummy {}, Engine::Derive_impl::Dummy {}));     \
        template <typename T>                                                                                                                                                                    \
        static auto derive(T &&t, Engine::Derive_impl::upToPrio<2>) -> decltype(std::forward<T>(t).Name(std::declval<Args>()..., Engine::Derive_impl::Dummy {}, Engine::Derive_impl::Dummy {})); \
    };                                                                                                                                                                                           \
    template <typename... Args>                                                                                                                                                                  \
    struct Derive_impl_##Name##_helper {                                                                                                                                                         \
        template <typename... Args2>                                                                                                                                                             \
        using type = Derive_impl_##Name##_def<Args..., Args2...>;                                                                                                                                \
    };                                                                                                                                                                                           \
    template <typename... Args>                                                                                                                                                                  \
    using Derive_impl_##Name = typename Derive_impl_##Name##_helper<__VA_ARGS__>::type<Args...>;                                                                                                 \
                                                                                                                                                                                                 \
    template <typename T, typename, typename prio, typename... Args>                                                                                                                             \
    struct has_function_##Name##_select : std::false_type {                                                                                                                                      \
        using return_type = void;                                                                                                                                                                \
    };                                                                                                                                                                                           \
                                                                                                                                                                                                 \
    template <typename T, typename prio, typename... Args>                                                                                                                                       \
    struct has_function_##Name##_select<T, void_t<decltype(Derive_impl_##Name<Args...>::derive(std::declval<T>(), prio {}))>, prio, Args...> : std::true_type {           \
        using return_type = decltype(Derive_impl_##Name<Args...>::derive(std::declval<T>(), prio {}));                                                                                           \
    };                                                                                                                                                                                           \
                                                                                                                                                                                                 \
    template <typename T, typename prio = Engine::Derive_impl::prio<0>, typename... Args>                                                                                                                                       \
    using has_function_##Name = has_function_##Name##_select<T, void, prio, Args...>;                                                                                                            \
    template <typename T, typename... Args>                                                                                                                                                      \
    constexpr bool has_function_##Name##_v = has_function_##Name<T, Engine::Derive_impl::prio<0>, Args...>::value;                                                                               \
    template <typename T, size_t delim, typename... Args>                                                                                                                                                      \
    constexpr bool has_function_##Name##_upTo_v = has_function_##Name<T, Engine::Derive_impl::upToPrio<delim>, Args...>::value;

#define DERIVE_TYPENAME(Name)                                                  \
    template <typename T, typename = void>                                     \
    struct has_typename_##Name : std::false_type {                             \
        template <typename Default>                                            \
        using type = Default;                                                  \
    };                                                                         \
    template <typename T>                                                      \
    struct has_typename_##Name<T, void_t<typename T::Name>> : std::true_type { \
        template <typename>                                                    \
        using type = typename T::Name;                                         \
    };                                                                         \
    template <typename T>                                                      \
    constexpr bool has_typename_##Name##_v = has_typename_##Name<T>::value;

}