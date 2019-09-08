#pragma once

namespace Engine {

struct Vector2i {
    int x, y;

    constexpr Vector2i operator+(const Vector2i &other)
    {
        return { x + other.x, y + other.y };
    }
};

}