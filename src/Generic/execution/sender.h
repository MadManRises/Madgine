#pragma once

namespace Engine {
namespace Execution {

    template <typename F, typename V, typename R>
    struct Sender : F {
        using F::operator();
    };

    template <typename V, typename R, typename F>
    Sender<F, V, R> make_sender(F &&f)
    {
        return { std::forward<F>(f) };
    }

#define ASYNC_STUB(Name, Impl, ...)                                                                                                                                                                         \
    template <typename... Args>                                                                                                                                                                             \
    auto Name(Args &&...args)                                                                                                                                                                               \
    {                                                                                                                                                                                                       \
        return __VA_ARGS__(LIFT(Impl, this), std::forward<Args>(args)...); \
    }

}
}