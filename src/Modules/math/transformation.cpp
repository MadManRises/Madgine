#include "../moduleslib.h"

#include "transformation.h"

#include "matrix4.h"
#include "quaternion.h"

namespace Engine {

Matrix4 TransformMatrix(const Vector3 &translate, const Vector3 &scale, const Quaternion &orientation)
{
    return TranslationMatrix(translate) * Matrix4 { orientation.toMatrix() * ScalingMatrix(scale) };
}

Matrix4 TranslationMatrix(const Vector3 &v)
{
    return { 1, 0, 0, v.x, 0, 1, 0, v.y, 0, 0, 1, v.z, 0, 0, 0, 1 };
}

Matrix3 ScalingMatrix(const Vector3 &v)
{
    return { v.x, 0, 0, 0, v.y, 0, 0, 0, v.z };
}

Matrix3 ExtractRotationMatrix(const Matrix3& m) {
    Vector3 scalings = { 1 / m.GetColumn(0).length(), 1 / m.GetColumn(1).length(), 1 / m.GetColumn(2).length() };
    return m * ScalingMatrix(scalings);
}

}