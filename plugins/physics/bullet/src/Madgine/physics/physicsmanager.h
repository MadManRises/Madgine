#pragma once

#include "Madgine/scene/scenecomponentbase.h"
#include "Madgine/scene/scenecomponentcollector.h"

#include "Generic/proxy.h"

#include "Meta/math/vector3.h"

#include "Generic/intervalclock.h"

#include "Modules/threading/customclock.h"

class btSoftRigidDynamicsWorld;
struct btSoftBodyWorldInfo;
class btManifoldPoint;
class btCollisionObject;
struct btCollisionObjectWrapper;

namespace Engine {
namespace Physics {

    struct PhysicsData;
    struct PhysicsListener;

    struct MADGINE_BULLET_EXPORT ContactPoint {

        ContactPoint(btManifoldPoint &point, const btCollisionObject *obj0, const btCollisionObject *obj1);

        void setFriction(float motion, const Vector3 &dir, const Vector3 &dir2);

        Vector3 normal1() const;

    private:
        btManifoldPoint &mPoint;
        const btCollisionObject *mObj0, *mObj1;
    };

    struct MADGINE_BULLET_EXPORT ContactPointIterator {

        ContactPointIterator(PhysicsData *data, int manifold, int index);

        PhysicsData *mData;
        int mManifold;
        int mIndex;

        ContactPoint operator*();
        Proxy<ContactPoint> operator->();
        void operator++();
        bool operator!=(const ContactPointIterator &other) const;

    private:
        void update();
    };

    struct MADGINE_BULLET_EXPORT ContactPointList {
        PhysicsData *mData;

        ContactPointIterator begin();
        ContactPointIterator end();
    };

    struct MADGINE_BULLET_EXPORT PhysicsManager : Scene::SceneComponent<PhysicsManager> {

        PhysicsManager(Scene::SceneManager &sceneMgr);
        ~PhysicsManager();

        btSoftRigidDynamicsWorld &world();
        btSoftBodyWorldInfo &worldInfo();

        ContactPointList contactPoints();

        void addListener(PhysicsListener *listener);
        void removeListener(PhysicsListener *listener);

        float airDensity() const;
        void setAirDensity(float density);

    protected:
        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        Threading::Task<void> update();

        static bool sContactCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0, int partId0, int index0, const btCollisionObjectWrapper *colObj1, int partId1, int index1);
        bool contactCallback(ContactPoint &p, RigidBody *body0, RigidBody *body1);

    private:
        std::unique_ptr<PhysicsData> mData;

        std::vector<PhysicsListener *> mListener;

        IntervalClock<Threading::CustomTimepoint> mClock;
    };

}
}

REGISTER_TYPE(Engine::Physics::PhysicsManager)