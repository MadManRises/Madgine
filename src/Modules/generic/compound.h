#pragma once

#include "templates.h"

namespace Engine {

template <typename... Ty>
struct Compound : Ty... {
    Compound() = default;
    Compound(typename Ty::ctor... args)
        : Ty({ args })...
    {
    }

    template <typename T>
    static const constexpr bool holds = type_pack_contains_v<type_pack<Ty...>, T>;
};

}