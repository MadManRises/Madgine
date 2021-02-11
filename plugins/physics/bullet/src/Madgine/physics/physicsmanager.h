#pragma once

#include "Madgine/scene/scenecomponentbase.h"
#include "Madgine/scene/scenecomponentcollector.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"

#include "Generic/proxy.h"

#include "Meta/math/vector3.h"

namespace Engine {
namespace Physics {

    struct PhysicsData;
    struct PhysicsListener;

    struct MADGINE_BULLET_EXPORT ContactPoint {

        void setFriction(float motion, const Vector3 &dir, const Vector3 &dir2);

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

        btDiscreteDynamicsWorld &world();

        ContactPointList contactPoints();

        void addListener(PhysicsListener *listener);
        void removeListener(PhysicsListener *listener);

    protected:
        virtual bool init() override;
        virtual void finalize() override;

        virtual void update(std::chrono::microseconds, bool paused) override;

        static bool sContactCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0, int partId0, int index0, const btCollisionObjectWrapper *colObj1, int partId1, int index1);
        bool contactCallback(ContactPoint &p);

    private:
        std::unique_ptr<PhysicsData> mData;
        
        std::vector<PhysicsListener *> mListener;
    };

}
}

RegisterType(Engine::Physics::PhysicsManager);