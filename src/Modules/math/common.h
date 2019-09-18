#pragma once

namespace Engine {

const constexpr float floatZeroThreshold = 1e-06f;

constexpr float abs(float f)
{
    return f < 0.0f ? -f : f;
}

constexpr bool isZero(float f)
{
    return abs(f) < floatZeroThreshold;
}

namespace Detail {
    constexpr float sqrtNewtonRaphson(float x, float curr, float prev)
    {
        return curr == prev
            ? curr
            : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
    }
}

constexpr float sqrtf(float x)
{
    return x >= 0 && x < std::numeric_limits<float>::infinity()
        ? Detail::sqrtNewtonRaphson(x, x, 0)
        : std::numeric_limits<float>::quiet_NaN();
}

template <typename T>
T min(const T& a, const T& b) {
    return a < b ? a : b;
}

template <typename T>
T max(const T &a, const T &b)
{
    return a > b ? a : b;
}

constexpr int sign(int i) {
    return i == 0 ? 0 : (i < 0 ? -1 : 1);
}

}