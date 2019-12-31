#pragma once

#include "templates.h"

namespace Engine {

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

#define DERIVE_TYPEDEF(Name)                                                  \
    template <typename T, typename = void>                                    \
    struct has_typedef_##Name : std::false_type {                             \
    };                                                                        \
    template <typename T>                                                     \
    struct has_typedef_##Name<T, void_t<typename T::Name>> : std::true_type { \
    };                                                                        \
    template <typename T>                                                     \
    constexpr bool has_typedef_##Name##_v = has_typedef_##Name<T>::value;

}