#pragma once

#include "../reflection/decay.h"

#include "../keyvalue/typedscopeptr.h"

namespace Engine {

template <typename _Base = ScopeBase>
struct VirtualScopeBase : _Base {
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

template struct MODULES_EXPORT VirtualScopeBase<>;

}
