#pragma once

namespace Engine
{
	namespace Scene
	{
		
		class Camera
		{
		public:
			virtual ~Camera() = default;

			virtual void setPosition(const Vector3 &pos) = 0;
			virtual Vector3 getPosition() const = 0;
		};

	}
}