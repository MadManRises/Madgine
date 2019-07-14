#include "../baselib.h"

#include "camera.h"

#include "Modules/math/matrix4.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

namespace Engine {
	namespace Scene {

		Camera::Camera() :
			mN(1.0f),
			mF(200.0f),
			mFOV(1.5708f),
			mPosition(0, 0, 4)
		{
		}

		void Camera::setPosition(const Vector3 & pos)
		{
			mPosition = pos;
		}

		const Vector3 & Camera::position()
		{
			return mPosition;
		}

		void Camera::setOrientation(const Quaternion & q)
		{
			mOrientation = q;
		}

		const Quaternion & Camera::orientation()
		{
			return mOrientation;
		}

		Matrix4 Camera::getViewProjectionMatrix(float aspectRatio)
		{
			Matrix4 rot = Matrix4(mOrientation.inverse().toMatrix());

			Matrix4 v = {
				1, 0, 0, -mPosition.x,
				0, 1, 0, -mPosition.y,
				0, 0, 1, -mPosition.z,
				0, 0, 0, 1
			};

			float t = tanf(mFOV / 2.0f) * mN;
			float r = t * aspectRatio;

			Matrix4 p = {
				mN / r,0,0,0,
				0,mN / t,0,0,
				0,0,-(mF + mN) / (mF - mN),2 * mF*mN / (mF - mN),
				0,0,1,0
			};

			return p * rot * v;

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

		/*KeyValueMapList Camera::maps()
		{
			return Scope::maps().merge(MAP(F, getF, setF), MAP(N, getN, setN), MAP(FOV, getFOV, setFOV), MAP(Position, position, setPosition), MAP(Rotate, getOrientationHandle, rotate));
		}*/

		Vector3 Camera::getOrientationHandle() const
		{
			return Vector3::ZERO;
		}

		void Camera::rotate(const Vector3 & rot)
		{
			mOrientation = Quaternion::FromRadian(rot) * mOrientation;
		}

		const std::vector<Entity::Entity*>& Camera::visibleEntities() const
		{
			return mVisibleEntities;
		}

		void Camera::setVisibleEntities(std::vector<Entity::Entity*>&& visibleEntities)
		{
			mVisibleEntities = std::move(visibleEntities);
		}

	}
}


METATABLE_BEGIN(Engine::Scene::Camera)
METATABLE_END(Engine::Scene::Camera)

RegisterType(Engine::Scene::Camera);
