#pragma once

namespace Engine {

DERIVE_FUNCTION(begin);
DERIVE_FUNCTION(end);
DERIVE_FUNCTION(rbegin);

template <typename T>
struct derive_iterator {
    using iterator = typename has_function_begin<T>::return_type;
    using const_iterator = typename has_function_begin<const T>::return_type;
    using reverse_iterator = typename has_function_rbegin<T>::return_type;
    using const_reverse_iterator = typename has_function_rbegin<const T>::return_type;
};

template <typename T>
using is_iterable = std::bool_constant<has_function_begin_v<T> && has_function_end_v<T>>;

template <typename T>
constexpr const bool is_iterable_v = is_iterable<T>::value;

}