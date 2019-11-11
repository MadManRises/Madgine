#pragma once

#include "vector2i.h"

namespace Engine {

struct Vector3i {
    int x, y, z;

	Vector3i() {}

    constexpr Vector3i(const int iX, const int iY, const int iZ)
        : x(iX)
        , y(iY)
        , z(iZ)
    {
    }

    constexpr Vector3i(const int afCoordinate[3])
        : x(afCoordinate[0])
        , y(afCoordinate[1])
        , z(afCoordinate[2])
    {
    }

    constexpr Vector3i operator+(const Vector3i &other)
    {
        return { x + other.x, y + other.y, z + other.z };
    }

    constexpr Vector3i &operator+=(const Vector3i &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    int operator[](const size_t i) const
    {
        assert(i < 3);

        return *(&x + i);
    }

    /** Swizzle-like narrowing operations
		*/
    Vector2i xy() const
    {
        return { x, y };
    }

    static const constexpr int ZERO[] { 0, 0, 0 };
};

}