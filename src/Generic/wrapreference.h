#pragma once

namespace Engine {

template <typename T>
struct wrap_reference {
    using type = T;
};

template <typename T>
struct wrap_reference<T &> {
    using type = std::reference_wrapper<T>;
};

template <typename T>
using wrap_reference_t = typename wrap_reference<T>::type;

}