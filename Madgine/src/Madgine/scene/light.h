#pragma once

#include "../serialize/serializableunit.h"
#include "../serialize/container/observed.h"
#include "../signalslot/slot.h"

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
			SignalSlot::Slot<&Light::onPowerChangedImpl> mPowerChangedSlot;
			SignalSlot::Slot<&Light::onPositionChangedImpl> mPositionChangedSlot;
		};
	}
}
