#pragma once

namespace Engine {

#define DERIVE_FRIEND(Name)         \
    template <typename T, typename> \
    friend struct has_function_##Name;

#define DERIVE_FUNCTION(Name)                                                                    \
    template <typename T, typename = void>                                                       \
    struct has_function_##Name : std::false_type {                                               \
        using return_type = void;                                                                \
    };                                                                                           \
    template <typename T>                                                                        \
    struct has_function_##Name<T, void_t<decltype(std::declval<T>().Name())>> : std::true_type { \
        using return_type = decltype(std::declval<T>().Name());                                  \
    };                                                                                           \
    template <typename T>                                                                        \
    constexpr bool has_function_##Name##_v = has_function_##Name<T>::value;

#define DERIVE_FUNCTION_ARGS2(Name, Name2, ...)                                                                           \
    template <typename T, typename = void>                                                                                \
    struct has_function_##Name2 : std::false_type {                                                                       \
        using return_type = void;                                                                                         \
    };                                                                                                                    \
    template <typename T>                                                                                                 \
    struct has_function_##Name2<T, void_t<std::invoke_result_t<decltype(&T::Name), T *, __VA_ARGS__>>> : std::true_type { \
        using return_type = std::invoke_result_t<decltype(&T::Name), T *, __VA_ARGS__>;                                   \
    };                                                                                                                    \
    template <typename T>                                                                                                 \
    constexpr bool has_function_##Name2##_v = has_function_##Name2<T>::value;

#define DERIVE_FUNCTION_ARGS(Name, ...) DERIVE_FUNCTION_ARGS2(Name, Name, __VA_ARGS__)

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