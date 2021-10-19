#pragma once

#include "typedscopeptr.h"

namespace Engine {

template <typename _Base = void>
struct VirtualScopeBase : _Base {
    virtual TypedScopePtr customScopePtr() = 0;
};

template <>
struct VirtualScopeBase<void> {
    virtual TypedScopePtr customScopePtr() = 0;
};

template <typename T, typename _Base = VirtualScopeBase<>>
struct VirtualScope : _Base {
    using _Base::_Base;
    virtual TypedScopePtr customScopePtr() override 
    {
        return { this, table<decayed_t<T>> };
    }
};

template struct META_EXPORT VirtualScopeBase<void>;

}
