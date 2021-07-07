#include "../clientlib.h"

#include "camera.h"

#include "Meta/math/matrix4.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/math/pi.h"

#include "Meta/math/ray.h"

#include "Meta/math/transformation.h"

METATABLE_BEGIN(Engine::Render::Camera)
CONSTRUCTOR()
MEMBER(mName)
MEMBER(mPosition)
MEMBER(mOrientation)
MEMBER(mN)
MEMBER(mF)
MEMBER(mFOV)
MEMBER(mOrthographic)
FUNCTION(getViewMatrix)
FUNCTION(getViewProjectionMatrix, aspectRatio)
METATABLE_END(Engine::Render::Camera)

namespace Engine {
namespace Render {

    Camera::Camera(std::string name)
        : mName(std::move(name))
    {
    }

    Matrix4 Camera::getViewProjectionMatrix(float aspectRatio)
    {
        return getProjectionMatrix(aspectRatio) * getViewMatrix();
    }

    Matrix4 Camera::getViewMatrix()
    {
        Matrix4 rotate = Matrix4 { mOrientation.inverse().toMatrix() };

        Matrix4 translate = TranslationMatrix(-mPosition);

        return rotate * translate;
    }

    Matrix4 Camera::getProjectionMatrix(float aspectRatio)
    {
        return getFrustum(aspectRatio).getProjectionMatrix();
    }

    Frustum Camera::getFrustum(float aspectRatio)
    {
        float r = tanf((mFOV / 180.0f * PI) / 2.0f) * mN;
        float t = r / aspectRatio;

        return { mPosition,
            mOrientation,
            t, r,
            mN, mF,
            mOrthographic };
    }

    Ray Camera::mousePointToRay(const Vector2 &mousePos, const Vector2 &viewportSize)
    {
        float aspectRatio = viewportSize.x / viewportSize.y;

        return getFrustum(aspectRatio).toRay(mousePos / viewportSize);
    }

    Ray Camera::toRay() const
    {
        Vector3 dir = mOrientation * Vector3 { 0, 0, mN };
        return { mPosition, dir };
    }

}
}
