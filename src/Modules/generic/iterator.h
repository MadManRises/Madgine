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

DERIVE_FUNCTION(begin);
DERIVE_FUNCTION(rbegin);

template <typename T>
struct derive_iterator {
    using iterator = typename has_function_begin<T>::return_type;
    using const_iterator = typename has_function_begin<const T>::return_type;
    using reverse_iterator = typename has_function_rbegin<T>::return_type;
    using const_reverse_iterator = typename has_function_rbegin<const T>::return_type;
};

}