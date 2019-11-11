#pragma once

namespace Engine {

struct Vector2i {
    int x, y;

    constexpr Vector2i &operator+=(const Vector2i &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector2i operator+(const Vector2i &other)
    {
        return { x + other.x, y + other.y };
    }

    constexpr bool operator==(const Vector2i &other) const
    {
        return x == other.x && y == other.y;
    }
};

}
