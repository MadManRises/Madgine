#pragma once

#include "typedscopeptr.h"

#include "../reflection/decay.h"

namespace Engine {
struct MODULES_EXPORT ScopeBase {
};

struct MODULES_EXPORT VirtualScopeBase : ScopeBase {
    virtual TypedScopePtr customScopePtr() = 0;
};

template <typename T, typename Base>
struct VirtualScope : Base {
    using Base::Base;
    virtual TypedScopePtr customScopePtr() override {
        return { this, &table<decayed_t<T>>() };
    }
};

}
