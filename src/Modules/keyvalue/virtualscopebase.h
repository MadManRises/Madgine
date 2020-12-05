#pragma once

#include "typedscopeptr.h"
#include "scopebase.h"

namespace Engine {

struct MODULES_EXPORT VirtualScopeBase : ScopeBase {
    inline virtual ~VirtualScopeBase() = default;
    virtual TypedScopePtr customScopePtr() = 0;
};

template <typename T>
struct VirtualScopeWrapper : VirtualScopeBase, T{
    template <typename... Args>
    VirtualScopeWrapper(Args &&... args)
        : T(std::forward<Args>(args)...)
    {
    }
    virtual TypedScopePtr customScopePtr() override {
        if constexpr (has_function_customScopePtr_v<T>)
            return T::customScopePtr();
        else
            return { static_cast<T*>(this), table<T> };
    }
};

}
