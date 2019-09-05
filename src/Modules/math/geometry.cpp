#include "../moduleslib.h"

#include "geometry.h"

#include "boundingbox.h"
#include "plane.h"
#include "ray.h"
#include "sphere.h"
#include "vector3.h"

#include "common.h"

namespace Engine {

float Distance(const Ray &ray, const Vector3 &point, float *rayClosestParameter)
{
    if (rayClosestParameter)
        *rayClosestParameter = ray.mDir.dotProduct(point - ray.mPoint);
    return ray.mDir.crossProduct(point - ray.mPoint).length();
}

bool Intersect(const Ray &ray, const Sphere &sphere, float *rayClosestParameter)
{
    Vector3 oc = ray.mPoint - sphere.mCenter;
    float a = ray.mDir.dotProduct(ray.mDir);
    float b = 2.0 * oc.dotProduct(ray.mDir);
    float c = oc.dotProduct(oc) - sphere.mRadius * sphere.mRadius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant >= 0) {
        if (rayClosestParameter)
            *rayClosestParameter = (-b - sqrt(discriminant)) / (2.0 * a);
        return true;
    } else {
        return false;
    }
}

bool Intersect(const Ray &ray, const BoundingBox &box, float *rayClosestParameter)
{
    Plane minX { box.mMin, box.mDirX };
    Plane maxX { box.mMax, box.mDirX };

    float tMin = std::numeric_limits<float>::lowest();
    float tMax = std::numeric_limits<float>::max();

    Intersect(minX, ray, &tMin);
    Intersect(maxX, ray, &tMax);

    if (tMin > tMax)
        std::swap(tMin, tMax);

    Plane minY { box.mMin, box.mDirY };
    Plane maxY { box.mMax, box.mDirY };

    float t2Min = std::numeric_limits<float>::lowest();
    float t2Max = std::numeric_limits<float>::max();

    Intersect(minY, ray, &t2Min);
    Intersect(maxY, ray, &t2Max);

    if (t2Min > t2Max)
        std::swap(t2Min, t2Max);

    tMin = std::max(tMin, t2Min);
    tMax = std::min(tMax, t2Max);

	if (tMin > tMax)
        return false;

    NormalizedVector3 dirZ = box.mDirX.crossProduct(box.mDirY);

	Plane minZ { box.mMin, dirZ };
    Plane maxZ { box.mMax, dirZ };

    t2Min = std::numeric_limits<float>::lowest();
    t2Max = std::numeric_limits<float>::max();

    Intersect(minZ, ray, &t2Min);
    Intersect(maxZ, ray, &t2Max);

    if (t2Min > t2Max)
        std::swap(t2Min, t2Max);

    tMin = std::max(tMin, t2Min);
    tMax = std::min(tMax, t2Max);

    if (tMin > tMax)
        return false;

	if (rayClosestParameter)
        *rayClosestParameter = tMin;

	return true;
}

bool Intersect(const Plane &plane, const Ray &ray, float *rayParameter)
{
    float denom = plane.mNormal.dotProduct(ray.mDir);
    if (!isZero(denom)) {
        Vector3 pDist = plane.mNormal * plane.mDistance - ray.mPoint;
        float t = pDist.dotProduct(plane.mNormal) / denom;
        if (rayParameter)
            *rayParameter = t;
        return true;
    }
    return false;
}

}