#include "../baselib.h"

#include "camera.h"

#include "Modules/math/matrix4.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/math/pi.h"

namespace Engine {
namespace Scene {

    Camera::Camera(std::string name)
        : mName(std::move(name))
        , mPosition(0, 0, 0)
        , mF(200.0f)
        , mN(0.1f)
        , mFOV(105)
    {
    }

    void Camera::setPosition(const Vector3 &pos)
    {
        mPosition = pos;
    }

    const Vector3 &Camera::position()
    {
        return mPosition;
    }

    void Camera::setOrientation(const Quaternion &q)
    {
        mOrientation = q;
    }

    const Quaternion &Camera::orientation()
    {
        return mOrientation;
    }

    Matrix4 Camera::getViewProjectionMatrix(float aspectRatio)
    {
        Matrix4 rotate = Matrix4(mOrientation.inverse().toMatrix());

        Matrix4 translate = Matrix4::TranslationMatrix(-mPosition);
            
		float r = tanf((mFOV / 180.0f * PI) / 2.0f) * mN;
        float t = r / aspectRatio;        

        Matrix4 p = {
            mN / r, 0, 0, 0,
            0, mN / t, 0, 0,
            0, 0, -(mF + mN) / (mF - mN), 2 * mF * mN / (mF - mN),
            0, 0, 1, 0
        };

        return p * rotate * translate;
    }

    float Camera::getF() const
    {
        return mF;
    }

    void Camera::setF(float f)
    {
        mF = f;
    }

    float Camera::getN() const
    {
        return mN;
    }

    void Camera::setN(float n)
    {
        mN = n;
    }

    float Camera::getFOV() const
    {
        return mFOV;
    }

    void Camera::setFOV(float fov)
    {
        mFOV = fov;
    }

    const std::string &Camera::getName() const
    {
        return mName;
    }

    void Camera::setName(const std::string &name)
    {
        mName = name;
    }

    Vector3 Camera::getOrientationHandle() const
    {
        return Vector3::ZERO;
    }

    void Camera::rotate(const Vector3 &rot)
    {
        mOrientation = Quaternion::FromRadian(rot) * mOrientation;
    }

    const std::vector<Entity::Entity *> &Camera::visibleEntities() const
    {
        return mVisibleEntities;
    }

    void Camera::setVisibleEntities(std::vector<Entity::Entity *> visibleEntities)
    {
        mVisibleEntities.swap(visibleEntities);
    }

    Ray Camera::mousePointToRay(const Vector2 &mousePos, const Vector2 &viewportSize)
    {
        float aspectRatio = viewportSize.x / viewportSize.y;

        float t = tanf(mFOV / 2.0f) * mN;
        float r = t * aspectRatio;

        Vector3 dir = mOrientation * Vector3 { (2.f * mousePos.x / viewportSize.x - 1.f) * r, (1.f - 2.f * mousePos.y / viewportSize.y) * t, mN };
        return { mPosition, dir };
    }

}
}

METATABLE_BEGIN(Engine::Scene::Camera)
PROPERTY(Name, getName, setName)
PROPERTY(Position, position, setPosition)
PROPERTY(Near, getN, setN)
PROPERTY(Far, getF, setF)
METATABLE_END(Engine::Scene::Camera)

RegisterType(Engine::Scene::Camera);
