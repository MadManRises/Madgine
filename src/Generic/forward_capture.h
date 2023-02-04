#pragma once

namespace Engine {

template <typename T>
struct CaptureHelper {
    operator T &()
    {
        return mT;
    }
    T mT;
};

template <typename T>
CaptureHelper<T> forward_capture(T &&t)
{
    return { std::forward<T>(t) };
}

}