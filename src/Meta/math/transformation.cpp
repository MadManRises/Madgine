#include "../metalib.h"

#include "transformation.h"

#include "matrix4.h"
#include "quaternion.h"
#include "frustum.h"

namespace Engine {

Matrix4 TransformMatrix(const Vector3 &translate, const Vector3 &scale, const Quaternion &orientation)
{
    return TranslationMatrix(translate) * Matrix4 { orientation.toMatrix() * ScalingMatrix(scale) };
}

std::tuple<Vector3, Vector3, Quaternion> DecomposeTransformMatrix(const Matrix4 &transform)
{
    std::tuple<Vector3, Vector3, Quaternion> result;
    std::get<0>(result) = transform.GetColumn(3).xyz();
    Matrix3 m3 = transform.ToMat3();
    std::get<1>(result) = { m3.GetColumn(0).length(), m3.GetColumn(1).length(), m3.GetColumn(2).length() };
    std::get<2>(result) = Quaternion::FromMatrix(ScalingMatrix(std::get<1>(result)).Inverse() * m3);
    return result;
}

Matrix4 TranslationMatrix(const Vector3 &v)
{
    return { 1, 0, 0, v.x, 0, 1, 0, v.y, 0, 0, 1, v.z, 0, 0, 0, 1 };
}

Matrix3 ScalingMatrix(const Vector3 &v)
{
    return { v.x, 0, 0, 0, v.y, 0, 0, 0, v.z };
}

Matrix4 ProjectionMatrix(const Frustum &frustum)
{
    float r = frustum.mRight;
    float t = frustum.mTop;
    float near = frustum.mNear;
    float far = frustum.mFar;

    Matrix4 p;

    if (frustum.mOrthographic) {
        p = {
            1 / r, 0, 0, 0,
            0, 1 / t, 0, 0,
            0, 0, 2 / (far - near), -(far + near) / (far - near),
            0, 0, 0, 1
        };
    } else {
        p = {
            near / r, 0, 0, 0,
            0, near / t, 0, 0,
            0, 0, (far + near) / (far - near), -2 * far * near / (far - near),
            0, 0, 1, 0
        };
    }

    return p;
}

Matrix3 ExtractRotationMatrix(const Matrix3& m) {
    
    return m * ExtractScalingMatrix(m).Inverse();
}

Matrix3 ExtractScalingMatrix(const Matrix3 &m)
{
    Vector3 scalings = { m.GetColumn(0).length(), m.GetColumn(1).length(), m.GetColumn(2).length() };
    return ScalingMatrix(scalings);
}

}