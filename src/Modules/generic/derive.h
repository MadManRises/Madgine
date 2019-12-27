#pragma once

#include "templates.h"

namespace Engine {

#define DERIVE_FUNCTION(Name)                                                   \
    template <typename T, typename = void>                                      \
    struct has_function_##Name {                                                \
        using return_type = void;                                               \
        static const constexpr bool value = false;                              \
    };                                                                          \
    template <typename T>                                                       \
    struct has_function_##Name<T, void_t<decltype(std::declval<T>().Name())>> { \
        using return_type = decltype(std::declval<T>().Name());                 \
        static const constexpr bool value = true;                               \
    };

#define DERIVE_TYPEDEF(Name)                                         \
    template <typename T, typename = void>                           \
    struct has_typedef_##Name : std::false_type {                    \
    };                                                               \
    template <typename T>                                            \
    struct has_typedef_##Name<T, void_t<typename T::Name>> : std::true_type { \
    };

}