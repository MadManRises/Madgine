#include "baselib.h"

#include "light.h"

namespace Engine {
	namespace Scene {

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

		float Light::power()
		{
			return mPower;
		}

		const Vector3& Light::position()
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
