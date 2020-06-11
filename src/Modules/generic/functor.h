#pragma once

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

}