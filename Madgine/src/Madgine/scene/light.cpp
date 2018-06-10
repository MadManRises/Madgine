#include "../baselib.h"

#include "light.h"
#include "scenemanager.h"

namespace Engine
{
	namespace Scene
	{
		Light::Light() :
			mPowerChangedSlot(this),
			mPositionChangedSlot(this)
		{
			mPower.setCallback(mPowerChangedSlot);
			mPosition.setCallback(mPositionChangedSlot);
		}

		void Light::setPower(float power)
		{
			mPower = power;
		}

		void Light::setPosition(const Vector3& position)
		{
			mPosition = position;
		}

		float Light::power() const
		{
			return mPower;
		}

		const Vector3& Light::position() const
		{
			return mPosition;
		}

		void Light::onPowerChanged(float power)
		{
		}

		void Light::onPositionChanged(const Vector3& position)
		{
		}
	}
}