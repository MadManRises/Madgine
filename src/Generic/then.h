#pragma once

#include "derive.h"

namespace Engine {

DERIVE_FUNCTION(then);

template <typename V, typename F>
decltype(auto) then(V &&v, F &&f)
{
    if constexpr (has_function_then_v<V&&, F&&>) {
        return std::forward<V>(v).then(std::forward<F>(f));
    } else {
        return std::forward<F>(f)(std::forward<V>(v));
    }
}

}