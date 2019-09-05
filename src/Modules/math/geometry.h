#pragma once

namespace Engine {

MODULES_EXPORT float Distance(const Ray &ray, const Vector3 &point, float *rayClosestParameter = nullptr);

MODULES_EXPORT bool Intersect(const Ray &ray, const Sphere &sphere, float *rayClosestParameter = nullptr);
MODULES_EXPORT bool Intersect(const Ray &ray, const BoundingBox &box, float *rayClosestParameter = nullptr);
MODULES_EXPORT bool Intersect(const Plane &plane, const Ray &ray, float *rayParameter = nullptr);

}