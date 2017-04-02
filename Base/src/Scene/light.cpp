#include "baselib.h"

#include "light.h"

namespace Engine {
	namespace Scene {

		Light::Light() :
			mPositionChangedSlot(this),
			mPowerChangedSlot(this)
		{
			mPower.setCallback(mPowerChangedSlot);
			mPosition.setCallback(mPositionChangedSlot);
		}

		void Light::setPower(float power)
		{
			mPower = power;
		}

		void Light::setPosition(const std::array<float, 3>& position)
		{
			mPosition = position;
		}

		float Light::power()
		{
			return mPower;
		}

		const std::array<float, 3>& Light::position()
		{
			return mPosition;
		}

		void Light::onPowerChanged(float power)
		{
		}

		void Light::onPositionChanged(const std::array<float, 3>& position)
		{
		}


	}
}
