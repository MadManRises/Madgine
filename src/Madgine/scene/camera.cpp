#include "../baselib.h"

#include "camera.h"

#include "Interfaces/math/matrix4.h"

#include "scenemanager.h"

#include "Interfaces/generic/keyvalueiterate.h"
#include "Interfaces/scripting/types/api.h"

namespace Engine {
	namespace Scene {

		Camera::Camera(SceneManager &scene) :
			Scope(&scene),
			mN(1.0f),
			mF(20.0f),
			mFOV(0.2f),
			mPosition(0, 0, -4)
		{
		}

		void Camera::setPosition(const Vector3 & pos)
		{
			mPosition = pos;
		}

		Vector3 Camera::getPosition()
		{
			return mPosition;
		}

		Matrix4 Camera::getViewProjectionMatrix(float aspectRatio)
		{
			Matrix4 rot = mOrientation.toMatrix();

			Matrix4 v = {
				1, 0, 0, -mPosition.x,
				0, 1, 0, -mPosition.y,
				0, 0, -1, mPosition.z,
				0, 0, 0, 1
			};

			float t = tanf(mFOV) * mN;
			float r = t * aspectRatio;

			Matrix4 p = {
				mN / r,0,0,0,
				0,mN / t,0,0,
				0,0,-(mF + mN) / (mF - mN),-2 * mF*mN / (mF - mN),
				0,0,-1,0
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

		KeyValueMapList Camera::maps()
		{
			return Scope::maps().merge(MAP(F, getF, setF), MAP(N, getN, setN), MAP(FOV, getFOV, setFOV), MAP(Position, getPosition, setPosition), MAP(Rotate, getOrientationHandle, setOrientationHandle));
		}

		Vector3 Camera::getOrientationHandle() const
		{
			return Vector3::ZERO;
		}

		void Camera::setOrientationHandle(const Vector3 & rot)
		{
			mOrientation = Quaternion(rot.x, Vector3::UNIT_Y) * Quaternion(rot.y, Vector3::UNIT_X) * Quaternion(rot.z, Vector3::UNIT_Z) * mOrientation;
		}

		const std::vector<Vertex>& Camera::vertices() const
		{
			return mCurrentVertices;
		}

		void Camera::setVertices(std::vector<Vertex>&& vertices)
		{
			mCurrentVertices = std::move(vertices);
		}

	}
}