#pragma once

namespace Engine {

struct NoOpFunctor {
    template <typename... Args>
    void operator()(Args &&...) { }
};

template <auto F>
struct Functor {
    template <typename... Args>
    requires requires { F(std::declval<Args>()...); }
    decltype(auto) operator()(Args &&...args)
    {
        return F(std::forward<Args>(args)...);
    }
};

}