#pragma once

namespace Engine {

template <typename T>
struct FundamentalWrapper {
	template <typename... Args>
    FundamentalWrapper(Args &&...args)
        : data(std::forward<Args>(args)...)
    {
    }

    operator T& () {
        return data;
    }

    T data;
};

template <typename T>
using Inheritable = std::conditional_t<std::is_class_v<T>, T, FundamentalWrapper<T>>;

}