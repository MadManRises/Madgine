#pragma once

namespace Engine {
namespace Physics {

    struct ContactPoint;

    struct PhysicsListener {
        virtual void onContact(ContactPoint &p) = 0;
    };

}
}