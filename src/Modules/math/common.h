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
}