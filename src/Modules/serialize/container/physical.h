#pragma once

#include "../../generic/derive.h"

namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION(physical);

    template <typename T>
    decltype(auto) physical(T &t)
    {
        if constexpr (has_function_physical_v<T>)
            return t.physical();
        else
            return t;
    }

}
}