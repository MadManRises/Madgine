#include "../clientlib.h"

#include "camera.h"

#include "Modules/math/matrix4.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/math/pi.h"

#include "Modules/math/ray.h"

#include "Modules/math/transformation.h"

namespace Engine {
namespace Render {

    Camera::Camera(std::string name)
        : mName(std::move(name))
        , mPosition(0, 0, 0)
        , mF(200.0f)
        , mN(0.1f)
        , mFOV(90)
    {
    }

    Matrix4 Camera::getViewProjectionMatrix(float aspectRatio)
    {
        return getProjectionMatrix(aspectRatio) * getViewMatrix();
    }

    Matrix4 Camera::getViewMatrix()
    {
        Matrix4 rotate = Matrix4(mOrientation.toMatrix());		

        Matrix4 translate = TranslationMatrix(-mPosition);

        return rotate * translate;
    }

    Matrix4 Camera::getProjectionMatrix(float aspectRatio)
    {
        float r = tanf((mFOV / 180.0f * PI) / 2.0f) * mN;
        float t = r /* / aspectRatio*/;

        Matrix4 p = {
            mN / r, 0, 0, 0,
            0, mN / t, 0, 0,
            0, 0, (mF + mN) / (mF - mN), -2 * mF * mN / (mF - mN),
            0, 0, 1, 0
        };

        return p;
    }

    Ray Camera::mousePointToRay(const Vector2 &mousePos, const Vector2 &viewportSize)
    {
        float aspectRatio = viewportSize.x / viewportSize.y;

        float r = tanf((mFOV / 180.0f * PI) / 2.0f) * mN;
        float t = r /* / aspectRatio*/;

        Vector3 dir = mOrientation * Vector3 { (2.f * mousePos.x / viewportSize.x - 1.0f) * r, (1.0f - 2.f * mousePos.y / viewportSize.y) * t, mN };
        dir.normalize();
        return { mPosition, dir };
    }

    Ray Camera::toRay() const
    {
        Vector3 dir = mOrientation * Vector3 { 0, 0, mN };
        return { mPosition, dir };
    }

}
}

METATABLE_BEGIN(Engine::Render::Camera)
MEMBER(mName)
MEMBER(mPosition)
MEMBER(mN)
MEMBER(mF)
MEMBER(mFOV)
METATABLE_END(Engine::Render::Camera)


