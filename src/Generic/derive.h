#pragma once

namespace Engine {

namespace __generic_impl__ {
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
    friend struct __VA_ARGS__ __derive_impl__##Name##_def;

#define DERIVE_FUNCTION2(Name, Fn, ...)                                                                                                                                                                    \
    template <typename... Args>                                                                                                                                                                            \
    struct __derive_impl__##Name##_def {                                                                                                                                                                   \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::prio<0>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()...));                                                                           \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::upToPrio<0>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()...));                                                                       \
                                                                                                                                                                                                           \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::prio<1>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()..., Engine::__generic_impl__::Dummy {}));                                        \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::upToPrio<1>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()..., Engine::__generic_impl__::Dummy {}));                                    \
                                                                                                                                                                                                           \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::prio<2>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()..., Engine::__generic_impl__::Dummy {}, Engine::__generic_impl__::Dummy {}));     \
        template <typename T>                                                                                                                                                                              \
        static auto derive(T &&t, Engine::__generic_impl__::upToPrio<2>) -> decltype(std::forward<T>(t).Fn(std::declval<Args>()..., Engine::__generic_impl__::Dummy {}, Engine::__generic_impl__::Dummy {})); \
    };                                                                                                                                                                                                     \
    template <typename... Args>                                                                                                                                                                            \
    struct __derive_impl__##Name##_helper {                                                                                                                                                                \
        template <typename... Args2>                                                                                                                                                                       \
        using type = __derive_impl__##Name##_def<Args..., Args2...>;                                                                                                                                       \
    };                                                                                                                                                                                                     \
    template <typename... Args>                                                                                                                                                                            \
    using __derive_impl__##Name = typename __derive_impl__##Name##_helper<__VA_ARGS__>::type<Args...>;                                                                                                     \
                                                                                                                                                                                                           \
    template <typename T, typename, typename prio, typename... Args>                                                                                                                                       \
    struct has_function_##Name##_select : std::false_type {                                                                                                                                                \
        using return_type = void;                                                                                                                                                                          \
    };                                                                                                                                                                                                     \
                                                                                                                                                                                                           \
    template <typename T, typename prio, typename... Args>                                                                                                                                                 \
    struct has_function_##Name##_select<T, std::void_t<decltype(__derive_impl__##Name<Args...>::derive(std::declval<T>(), prio {}))>, prio, Args...> : std::true_type {                                    \
        using return_type = decltype(__derive_impl__##Name<Args...>::derive(std::declval<T>(), prio {}));                                                                                                  \
    };                                                                                                                                                                                                     \
                                                                                                                                                                                                           \
    template <typename T, typename prio = Engine::__generic_impl__::prio<0>, typename... Args>                                                                                                              \
    using has_function_##Name = has_function_##Name##_select<T, void, prio, Args...>;                                                                                                                      \
    template <typename T, typename... Args>                                                                                                                                                                \
    constexpr bool has_function_##Name##_v = has_function_##Name<T, Engine::__generic_impl__::prio<0>, Args...>::value;                                                                                     \
    template <typename T, size_t delim, typename... Args>                                                                                                                                                  \
    constexpr bool has_function_##Name##_upTo_v = has_function_##Name<T, Engine::__generic_impl__::upToPrio<delim>, Args...>::value;

#define DERIVE_FUNCTION(Name, ...) DERIVE_FUNCTION2(Name, Name, __VA_ARGS__)

#define DERIVE_OPERATOR(Name, Operator)                                                                                                                         \
    template <typename Arg>                                                                                                                                     \
    struct __derive_impl__##Name {                                                                                                                              \
        template <typename T>                                                                                                                                   \
        static auto derive(T &&t, Engine::__generic_impl__::upToPrio<0>) -> decltype(std::forward<T>(t).operator Operator(std::declval<Arg>()));                 \
                                                                                                                                                                \
        template <typename T>                                                                                                                                   \
        static auto derive(T &&t, Engine::__generic_impl__::upToPrio<1>) -> decltype(operator Operator(std::forward<T>(t), std::declval<Arg>()));                \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template <typename T, typename, typename prio, typename Arg>                                                                                                \
    struct has_operator_##Name##_select : std::false_type {                                                                                                     \
        using return_type = void;                                                                                                                               \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template <typename T, typename prio, typename Arg>                                                                                                          \
    struct has_operator_##Name##_select<T, std::void_t<decltype(__derive_impl__##Name<Arg>::derive(std::declval<T>(), prio {}))>, prio, Arg> : std::true_type { \
        using return_type = decltype(__derive_impl__##Name<Arg>::derive(std::declval<T>(), prio {}));                                                           \
    };                                                                                                                                                          \
                                                                                                                                                                \
    template <typename T, typename Arg>                                                                                                                         \
    using has_operator_##Name = has_operator_##Name##_select<T, void, Engine::__generic_impl__::upToPrio<1>, Arg>;                                               \
    template <typename T, typename Arg>                                                                                                                         \
    constexpr bool has_operator_##Name##_v = has_operator_##Name<T, Arg>::value;

#define DERIVE_TYPENAME(Name)                                                       \
    template <typename T, typename = void>                                          \
    struct has_typename_##Name : std::false_type {                                  \
        template <typename Default>                                                 \
        using type = Default;                                                       \
    };                                                                              \
    template <typename T>                                                           \
    struct has_typename_##Name<T, std::void_t<typename T::Name>> : std::true_type { \
        template <typename>                                                         \
        using type = typename T::Name;                                              \
    };                                                                              \
    template <typename T>                                                           \
    constexpr bool has_typename_##Name##_v = has_typename_##Name<T>::value;

}