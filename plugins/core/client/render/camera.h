#pragma once

#include "Interfaces/math/vector3.h"
#include "Interfaces/math/quaternion.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine
{
	namespace Render
	{
		
		class MADGINE_CLIENT_EXPORT Camera : public Scripting::Scope<Camera>
		{
		public:
			Camera(RendererBase *renderer);
			virtual ~Camera() = default;

			void setPosition(const Vector3 &pos);
			Vector3 getPosition();

			void addTargetWindow(GUI::SceneWindow *window);
			void removeTargetWindow(GUI::SceneWindow *window);

			Matrix4 getViewProjectionMatrix(float aspectRatio);

		protected:
			float getF() const;
			void setF(float f);
			float getN() const;
			void setN(float n);
			float getFOV() const;
			void setFOV(float fov);

			KeyValueMapList maps() override;

			Vector3 getOrientationHandle() const;
			void setOrientationHandle(const Vector3 &rot);

		private:
			Vector3 mPosition;
			Quaternion mOrientation;

			float mF;
			float mN;
			float mFOV;

			std::vector<GUI::SceneWindow*> mTargetWindows;

		};

	}
}

RegisterType(Engine::Render::Camera);