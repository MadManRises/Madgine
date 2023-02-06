#pragma once

#include "functor.h"

namespace Engine {

template <typename F, typename... Args>
struct Pipable {
    template <typename Ty>
    friend decltype(auto) operator|(Ty &&ty, Pipable &&p)
    {
        return TupleUnpacker::invokeExpand(p.mF, std::forward<Ty>(ty), std::move(p.mArgs));
    }
    F mF;
    std::tuple<Args...> mArgs;
};

template <typename F, typename... Args>
Pipable<F, Args...> pipable(F &&f, Args &&...args)
{
    return { std::forward<F>(f), std::forward<Args>(args)... };
}

#define MAKE_PIPABLE(f)                                       \
    template <typename... Args>                               \
    auto f(Args &&...args)                                    \
    {                                                         \
        return pipable(LIFT(f), std::forward<Args>(args)...); \
    }

}