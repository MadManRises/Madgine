#pragma once

#include "Interfaces/math/vector3.h"
#include "Interfaces/math/vector4.h"
#include "Interfaces/math/quaternion.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine
{
	namespace Scene
	{
		struct Vertex {
			Vector3 mPos;
			Vector4 mColor;
			Vector3 mNormal;
		};
		
		class MADGINE_BASE_EXPORT Camera : public Scripting::Scope<Camera>
		{
		public:
			Camera(SceneManager &scene);
			~Camera() = default;

			void setPosition(const Vector3 &pos);
			Vector3 getPosition();

			Matrix4 getViewProjectionMatrix(float aspectRatio);

			float getF() const;
			void setF(float f);
			float getN() const;
			void setN(float n);
			float getFOV() const;
			void setFOV(float fov);

			KeyValueMapList maps() override;

			Vector3 getOrientationHandle() const;
			void setOrientationHandle(const Vector3 &rot);

			const std::vector<Vertex> &vertices() const;
			void setVertices(std::vector<Vertex> &&vertices);

		private:
			Vector3 mPosition;
			Quaternion mOrientation;

			float mF;
			float mN;
			float mFOV;

			std::vector<Vertex> mCurrentVertices;

		};

	}
}

RegisterType(Engine::Scene::Camera);