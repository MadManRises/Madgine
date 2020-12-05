#pragma once

#include "../keyvalue/virtualscopebase.h"

#include "../reflection/decay.h"


namespace Engine {

template <typename T, typename _Base = VirtualScopeBase>
struct VirtualScope : _Base {
    using _Base::_Base;
    virtual ~VirtualScope() = default;
    virtual TypedScopePtr customScopePtr() override
    {
        return { this, table<decayed_t<T>> };
    }
};

}
