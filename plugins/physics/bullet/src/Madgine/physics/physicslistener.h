#pragma once

namespace Engine {
namespace Physics {

    struct ContactPoint;
    struct RigidBody;

    struct PhysicsListener {
        virtual void onContact(ContactPoint &p, RigidBody *body0, RigidBody *body1) = 0;
    };

}
}