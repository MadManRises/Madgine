#pragma once

#include "../generic/upto.h"

namespace Engine {

MODULES_EXPORT float Distance(const Ray &ray, const Vector3 &point, float *rayClosestParameter = nullptr);

MODULES_EXPORT UpTo<float, 2> Intersect(const Ray &ray, const Sphere &sphere);
MODULES_EXPORT UpTo<float, 2> Intersect(const Ray &ray, const BoundingBox &box);
MODULES_EXPORT UpTo<float, 1> Intersect(const Ray &ray, const Plane &plane);

}