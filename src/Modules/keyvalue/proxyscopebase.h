#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT ProxyScopeBase {
    inline virtual ~ProxyScopeBase() = default;
    virtual TypedScopePtr proxyScopePtr() = 0;
};

template <typename T>
struct ScopeWrapper : ProxyScopeBase, T {
    template <typename... Args>
    ScopeWrapper(Args &&... args)
        : T(std::forward<Args>(args)...)
    {
    }
    virtual TypedScopePtr proxyScopePtr() override
    {
        return static_cast<T*>(this);
    }
};

}
