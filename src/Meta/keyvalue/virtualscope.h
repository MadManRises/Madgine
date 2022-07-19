#pragma once

#include "typedscopeptr.h"

namespace Engine {

template <typename Base = void>
struct VirtualScopeBase : Base {
    virtual TypedScopePtr customScopePtr() = 0;
};

template <>
struct VirtualScopeBase<void> {
    virtual TypedScopePtr customScopePtr() = 0;
};

template <typename T, typename Base = VirtualScopeBase<>>
struct VirtualScope : Base {
    using Base::Base;
    virtual TypedScopePtr customScopePtr() override 
    {
        return { this, table<decayed_t<T>> };
    }
};

template struct META_EXPORT VirtualScopeBase<void>;

}
