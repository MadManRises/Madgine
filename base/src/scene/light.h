#pragma once

#include "serialize/serializableunit.h"
#include "serialize/container/observed.h"
#include "signalslot/slot.h"

namespace Engine {
	namespace Scene {

		class MADGINE_BASE_EXPORT Light : public Engine::Serialize::SerializableUnit<Light> {
		public:
			Light();
			virtual ~Light() = default;

			void setPower(float power);
			void setPosition(const std::array<float, 3> &position);

			float power();
			const std::array<float, 3> &position();
		protected:
			virtual void onPowerChanged(float power);
			virtual void onPositionChanged(const std::array<float, 3> &position);

			void onPowerChangedImpl(float power) {
				onPowerChanged(power);
			}
			void onPositionChangedImpl(const std::array<float, 3> &position) {
				onPositionChanged(position);
			}

		private:			

			Engine::Serialize::Observed<std::array<float, 3>> mPosition;
			Engine::Serialize::Observed<float> mPower;
			Engine::SignalSlot::Slot<decltype(&Light::onPowerChangedImpl), &Light::onPowerChangedImpl> mPowerChangedSlot;
			Engine::SignalSlot::Slot<decltype(&Light::onPositionChangedImpl), &Light::onPositionChangedImpl> mPositionChangedSlot;
		};

	}
}