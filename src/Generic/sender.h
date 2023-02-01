#pragma once

namespace Engine {

template <typename F, typename R>
struct Sender : F {
    using result_type = R;

    using F::operator();
};

template <typename R, typename F>
Sender<F, R> make_sender(F&& f) {
    return { std::forward<F>(f) };
}

}