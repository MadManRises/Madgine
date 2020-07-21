#pragma once

#include "offsetptr.h"

namespace Engine {

template <auto F>
struct Functor {
    template <typename... Ty>
    decltype(auto) operator()(Ty &&... args)
    {
        return F(std::forward<Ty>(args)...);
    }
};

template <auto F>
struct MemberFunctor {
    template <typename T, typename... Ty>
    decltype(auto) operator()(T *t, Ty &&... args)
    {
        return (t->*F)(std::forward<Ty>(args)...);
    }
};

template <auto f, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
struct ParentFunctor {
    template <typename... Args>
    decltype(auto) operator()(Args &&... args)
    {
        return (OffsetPtr::parent(this)->*f)(std::forward<Args>(args)...);
    }
};

}