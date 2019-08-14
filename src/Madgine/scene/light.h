#pragma once

#include "Modules/serialize/serializableunit.h"
#include "Modules/serialize/container/observed.h"
#include "Modules/threading/slot.h"
#include "Modules/math/vector3.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_BASE_EXPORT Light : public Serialize::SerializableUnit<Light>
		{
		public:
			Light();
			virtual ~Light() = default;

			void setPower(float power);
			void setPosition(const Vector3& position);

			float power() const;
			const Vector3& position() const;
		protected:
			virtual void onPowerChanged(float power);
			virtual void onPositionChanged(const Vector3& position);

			void onPowerChangedImpl(float power)
			{
				onPowerChanged(power);
			}

			void onPositionChangedImpl(const Vector3& position)
			{
				onPositionChanged(position);
			}

		private:

			Serialize::Observed<Vector3> mPosition;
			Serialize::Observed<float> mPower;
			Threading::Slot<&Light::onPowerChangedImpl> mPowerChangedSlot;
                        Threading::Slot<&Light::onPositionChangedImpl> mPositionChangedSlot;
		};
	}
}
