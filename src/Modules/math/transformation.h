#pragma once

namespace Engine {

MODULES_EXPORT Matrix4 TransformMatrix(const Vector3 &translate, const Vector3 &scale, const Quaternion &orientation);

MODULES_EXPORT Matrix4 TranslationMatrix(const Vector3 &v);
MODULES_EXPORT Matrix3 ScalingMatrix(const Vector3 &v);

MODULES_EXPORT Matrix3 ExtractRotationMatrix(const Matrix3 &m);
MODULES_EXPORT Matrix3 ExtractScalingMatrix(const Matrix3 &m);

}