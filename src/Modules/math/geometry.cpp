#include "../moduleslib.h"

#include "geometry.h"

#include "ray.h"
#include "sphere.h"
#include "plane.h"
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