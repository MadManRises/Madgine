#pragma once

#include "color3.h"
#include "vector4.h"

namespace Engine {

struct Color4 {

    Color4() { }

    constexpr Color4(float r, float g, float b, float a)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    constexpr Color4(const Color3 &c, float a)
        : r(c.r)
        , g(c.g)
        , b(c.b)
        , a(a)
    {
    }

    constexpr Color4(float *const r)
        : r(r[0])
        , g(r[1])
        , b(r[2])
        , a(r[3])
    {
    }

    constexpr operator Vector4() const
    {
        const auto toLinear = [](float x) {
            if (x <= 0.04045f) {
                return x / 12.92f;
            } else {
                return std::pow((x + 0.055f) / 1.055f, 2.4f);
            }
        };
        return { toLinear(r), toLinear(g), toLinear(b), a };
    }

    friend std::ostream &operator<<(std::ostream &o, const Color4 &c)
    {
        o << "(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
        return o;
    }

    friend std::istream &operator>>(std::istream &in, Color4 &color)
    {
        char c;
        in >> c;
        if (c != '(')
            std::terminate();
        for (int i = 0; i < 4; ++i) {
            in >> (&color.r)[i];
            in >> c;
            if (i != 3) {
                if (c != ',')
                    std::terminate();
            } else {
                if (c != ')')
                    std::terminate();
            }
        }
        return in;
    }

    float r;
    float g;
    float b;
    float a;
};

}