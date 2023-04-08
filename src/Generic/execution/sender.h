#pragma once

namespace Engine {
namespace Execution {

    template <typename F, typename R, typename... V>
    struct Sender : F {
        using F::operator();

        using result_type = R;
    };

    template <typename R, typename... V, typename F>
    auto make_sender(F &&f)
    {
        if constexpr (std::same_as<type_pack<V...>, type_pack<void>>)
            return make_sender<R>(std::forward<F>(f));
        else
            return Sender<F, R, V...> { std::forward<F>(f) };
    }

#define ASYNC_STUB(Name, Impl, ...)                                                                                                                                                                         \
    template <typename... Args>                                                                                                                                                                             \
    auto Name(Args &&...args)                                                                                                                                                                               \
    {                                                                                                                                                                                                       \
        return __VA_ARGS__(LIFT(Impl, this), std::forward<Args>(args)...); \
    }

}
}