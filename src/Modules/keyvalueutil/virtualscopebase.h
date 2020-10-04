#pragma once

#include "../keyvalue/typedscopeptr.h"

#include "../reflection/decay.h"

#include "../keyvalue/scopebase.h"

namespace Engine {

template <typename _Base = std::monostate>
struct DLL_EXPORT VirtualScopeBase : _Base, ScopeBase {
    using _Base::_Base;
    inline virtual ~VirtualScopeBase() = default;
    virtual TypedScopePtr customScopePtr() = 0;
};

template <typename T, typename _Base = VirtualScopeBase<>>
struct VirtualScope : _Base {
    using _Base::_Base;
    virtual ~VirtualScope() = default;
    virtual TypedScopePtr customScopePtr() override
    {
        return { this, table<decayed_t<T>> };
    }
};

}
