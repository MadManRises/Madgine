#pragma once

#include "derive.h"

namespace Engine {

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