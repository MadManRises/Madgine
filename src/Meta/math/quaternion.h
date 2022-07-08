#pragma once

#include "matrix3.h"
#include "pi.h"
#include "vector3.h"

namespace Engine {

struct Quaternion {

    Quaternion()
        : v(Vector3::ZERO)
        , w(1.0f)
    {
    }

    Quaternion(float radian, const Vector3 &axis)
        : v(axis)
        , w(cosf(0.5f * radian))
    {
        v.normalize();
        v *= sinf(0.5f * radian);
    }

    Quaternion(float x, float y, float z, float w)
        : v(x, y, z)
        , w(w)
    {
        normalize();
    }

    constexpr explicit Quaternion(float *const r)
        : v(r)
        , w(r[3])
    {
    }

    static Quaternion FromRadian(const Vector3 &angles)
    {
        return Quaternion(angles.z, Vector3::UNIT_Z) * Quaternion(angles.y, Vector3::UNIT_Y) * Quaternion(angles.x, Vector3::UNIT_X);
    }

    static Quaternion FromDegrees(const Vector3 &angles)
    {
        return FromRadian(angles / 180.0f * PI);
    }

    Vector3 toDegrees() const
    {
        return {
            std::atan2(2 * (w * v.x + v.y * v.z), 1 - 2 * (v.x * v.x + v.y * v.y)) / PI * 180.0f,
            std::asin(2 * (w * v.y - v.z * v.x)) / PI * 180.0f,
            std::atan2(2 * (w * v.z + v.x * v.y), 1 - 2 * (v.y * v.y + v.z * v.z)) / PI * 180.0f
        };
    }

    static Quaternion FromDirection(const NormalizedVector3 &dir, const NormalizedVector3 &up = { Vector3::UNIT_Y })
    {
        NormalizedVector3 side = up.crossProduct(dir);
        NormalizedVector3 actualUp = dir.crossProduct(side);

        Matrix3 m;
        m.FromAxes(side, actualUp, dir);
        return FromMatrix(m);
    }

    static Quaternion FromMatrix(const Matrix3 &m)
    {
        float qw, qx, qy, qz;
        float tr = m[0][0] + m[1][1] + m[2][2];

        if (tr > 0) {
            float S = sqrt(tr + 1.0) * 2; // S=4*qw
            qw = 0.25 * S;
            qx = (m[2][1] - m[1][2]) / S;
            qy = (m[0][2] - m[2][0]) / S;
            qz = (m[1][0] - m[0][1]) / S;
        } else if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) {
            float S = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2; // S=4*qx
            qw = (m[2][1] - m[1][2]) / S;
            qx = 0.25 * S;
            qy = (m[0][1] + m[1][0]) / S;
            qz = (m[0][2] + m[2][0]) / S;
        } else if (m[1][1] > m[2][2]) {
            float S = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2; // S=4*qy
            qw = (m[0][2] - m[2][0]) / S;
            qx = (m[0][1] + m[1][0]) / S;
            qy = 0.25 * S;
            qz = (m[1][2] + m[2][1]) / S;
        } else {
            float S = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2; // S=4*qz
            qw = (m[1][0] - m[0][1]) / S;
            qx = (m[0][2] + m[2][0]) / S;
            qy = (m[1][2] + m[2][1]) / S;
            qz = 0.25 * S;
        }

        assert(!isnan(qx) && !isnan(qy) && !isnan(qz) && !isnan(qw));
        return { qx, qy, qz, qw };
    }

    void operator*=(const Quaternion &other)
    {
        float newW = w * other.w - v.dotProduct(other.v);
        v = w * other.v + other.w * v + v.crossProduct(other.v);
        w = newW;
    }

    Quaternion operator*(const Quaternion &other) const
    {
        Quaternion q { *this };
        q *= other;
        return q;
    }

    void operator+=(const Quaternion &other)
    {
        w += other.w;
        v += other.v;
    }

    Quaternion operator+(const Quaternion &other)
    {
        Quaternion q { *this };
        q += other;
        return q;
    }

    void operator-=(const Quaternion &other)
    {
        w -= other.w;
        v -= other.v;
    }

    Quaternion operator-(const Quaternion &other)
    {
        Quaternion q { *this };
        q -= other;
        return q;
    }

    void operator*=(float c)
    {
        v *= c;
        w *= c;
    }

    Quaternion operator*(float c)
    {
        Quaternion q { *this };
        q *= c;
        return q;
    }

    float dotProduct(const Quaternion &other)
    {
        return w * other.w + v.dotProduct(other.v);
    }

    void invert()
    {
        v *= -1;
    }

    Quaternion inverse() const
    {
        Quaternion q { *this };
        q.invert();
        return q;
    }

    Matrix3 toMatrix() const
    {
        return Matrix3 {
            1 - 2 * (v.y * v.y + v.z * v.z), 2 * (v.x * v.y - v.z * w), 2 * (v.x * v.z + v.y * w),
            2 * (v.x * v.y + v.z * w), 1 - 2 * (v.x * v.x + v.z * v.z), 2 * (v.y * v.z - v.x * w),
            2 * (v.x * v.z - v.y * w), 2 * (v.y * v.z + v.x * w), 1 - 2 * (v.x * v.x + v.y * v.y)
        };
    }

    Vector3 operator*(const Vector3 &dir) const
    {
        return toMatrix() * dir;
    }

    bool operator==(const Quaternion &other) const
    {
        return w == other.w && v == other.v;
    }

    Quaternion operator-() const
    {
        return inverse();
    }

    void normalize()
    {
        float invNorm = 1.0f / sqrtf(w * w + v.squaredLength());
        v *= invNorm;
        w *= invNorm;
    }

    Vector3 v;
    float w;

    friend std::ostream &operator<<(std::ostream &o, const Quaternion &q)
    {
        o << "{" << q.v.x << ", " << q.v.y << ", " << q.v.z << ", " << q.w << "}";
        return o;
    }
};

META_EXPORT Quaternion slerp(Quaternion q1, Quaternion q2, float ratio);
}