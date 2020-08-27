#pragma once

namespace Engine {

struct Vector2i {
    int x, y;

    Vector2i() = default;

    constexpr Vector2i(int x, int y)
        : x(x)
        , y(y)
    {
    }

    constexpr Vector2i(const InterfacesVector &v)
        : x(v.x)
        , y(v.y)
    {
    }

    constexpr operator InterfacesVector() const {
        return { x, y };
    }

    constexpr Vector2i(const Vector2i &) = default;

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

    constexpr Vector2i operator-(const Vector2i &other)
    {
        return { x - other.x, y - other.y };
    }

    constexpr bool operator==(const Vector2i &other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr int operator[](const size_t i) const
    {
        assert(i < 2);

        return *(&x + i);
    }

    constexpr int &operator[](const size_t i)
    {
        assert(i < 2);

        return *(&x + i);
    }

    friend std::ostream &operator<<(std::ostream &o, const Vector2i &v)
    {
        o << "[" << v.x << ", " << v.y << "]";
        return o;
    }

    friend std::istream &operator>>(std::istream &in, Vector2i &v)
    {
        char c;
        in >> c;
        if (c != '[')
            std::terminate();
        for (int i = 0; i < 2; ++i) {
            in >> v[i];
            in >> c;
            if (i != 1) {
                if (c != ',')
                    std::terminate();
            } else {
                if (c != ']')
                    std::terminate();
            }
        }
        return in;
    }
};

}
