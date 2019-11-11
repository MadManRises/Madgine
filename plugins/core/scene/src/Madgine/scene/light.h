#pragma once

#include "Modules/math/vector3.h"
#include "Modules/serialize/container/synced.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/signalslot/signalfunctor.h"
#include "Modules/threading/slot.h"

namespace Engine {
namespace Scene {
    class MADGINE_SCENE_EXPORT Light : public Serialize::SerializableUnit<Light> {
    public:
        Light();
        virtual ~Light() = default;

        void setPower(float power);
        void setPosition(const Vector3 &position);

        float power() const;
        const Vector3 &position() const;

    protected:
        /*virtual void onPowerChanged(float power);
        virtual void onPositionChanged(const Vector3 &position);

        void onPowerChangedImpl(float power)
        {
            onPowerChanged(power);
        }

        void onPositionChangedImpl(const Vector3 &position)
        {
            onPositionChanged(position);
        }*/

    private:
        SYNCED(mPosition, Vector3);
        SYNCED(mPower, float);
        //Serialize::Observed<int, Vector3> mPosition;
        //Serialize::Observed<int, float> mPower;
        /*Threading::Slot<&Light::onPowerChangedImpl> mPowerChangedSlot;
        Threading::Slot<&Light::onPositionChangedImpl> mPositionChangedSlot;*/
    };
}
}
