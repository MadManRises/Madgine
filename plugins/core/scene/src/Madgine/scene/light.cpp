#include "../scenelib.h"

#include "light.h"

#include "Modules/serialize/serializetable_impl.h"


namespace Engine
{
	namespace Scene
	{
		Light::Light()/* :
			mPowerChangedSlot(this),
			mPositionChangedSlot(this)*/
		{
			/*mPower.observer().signal().connect(mPowerChangedSlot);
			mPosition.observer().signal().connect(mPositionChangedSlot);*/
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

		/*void Light::onPowerChanged(float power)
		{
		}

		void Light::onPositionChanged(const Vector3& position)
		{
		}*/
	}
}

SERIALIZETABLE_BEGIN(Engine::Scene::Light)
SERIALIZETABLE_END(Engine::Scene::Light)