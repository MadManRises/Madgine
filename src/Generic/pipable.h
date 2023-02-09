#pragma once

#include "functor.h"
#include "lift.h"

namespace Engine {

template <typename F, typename... Args>
struct Pipable {
    template <typename Ty>
    friend decltype(auto) operator|(Ty &&ty, Pipable &&p)
    {
        return p.call(std::forward<Ty>(ty), std::make_index_sequence<sizeof...(Args)> {});
    }

    template <typename Ty, size_t... I>
    decltype(auto) call(Ty&& ty, std::index_sequence<I...>) {
        return std::move(mF)(std::forward<Ty>(ty), std::get<I>(std::move(mArgs))...);
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
        return ::Engine::pipable(LIFT(f), std::forward<Args>(args)...); \
    }

}