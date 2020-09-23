#include "../bulletlib.h"

#include "physicsmanager.h"

#include "rigidbody.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

UNIQUECOMPONENT(Engine::Physics::PhysicsManager)

METATABLE_BEGIN(Engine::Physics::PhysicsManager)
METATABLE_END(Engine::Physics::PhysicsManager)

SERIALIZETABLE_BEGIN(Engine::Physics::PhysicsManager)
SERIALIZETABLE_END(Engine::Physics::PhysicsManager)

namespace Engine {
namespace Physics {

    struct PhysicsData {
        btDefaultCollisionConfiguration mCollisionConfiguration;
        btCollisionDispatcher mDispatcher;
        btDbvtBroadphase mBroadphase;
        btSequentialImpulseConstraintSolver mSolver;
        btDiscreteDynamicsWorld mWorld;

        PhysicsData()
            : mDispatcher(&mCollisionConfiguration)
            , mWorld(&mDispatcher, &mBroadphase, &mSolver, &mCollisionConfiguration)
        {
            mWorld.setGravity({ 0, -9.81f, 0 });

            mWorld.getSolverInfo().m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
        }
    };

    PhysicsManager::PhysicsManager(Scene::SceneManager &sceneMgr)
        : VirtualScope(sceneMgr)
    {
    }

    PhysicsManager::~PhysicsManager()
    {
    }

    btDiscreteDynamicsWorld &PhysicsManager::world()
    {
        return mData->mWorld;
    }

    ContactPointList PhysicsManager::contactPoints()
    {
        return { mData.get() };
    }

    bool PhysicsManager::init()
    {
        mData = std::make_unique<PhysicsData>();

        return VirtualScope::init();
    }

    void PhysicsManager::finalize()
    {
        mData.reset();

        VirtualScope::finalize();
    }

    void PhysicsManager::update(std::chrono::microseconds timeSinceLastFrame)
    {
        mData->mWorld.stepSimulation(timeSinceLastFrame.count() / 1000000.0f, 1, 1.0f / 30.0f);

        for (RigidBody &rigidBody : sceneMgr().entityComponentList<RigidBody>()) {
            rigidBody.update();
        }
    }

    ContactPointIterator ContactPointList::begin()
    {
        return { mData, 0, 0 };
    }

    ContactPointIterator ContactPointList::end()
    {
        return { mData, mData->mDispatcher.getNumManifolds(), 0 };
    }

    ContactPointIterator::ContactPointIterator(PhysicsData *data, int manifold, int index)
        : mData(data)
        , mManifold(manifold)
        , mIndex(index)
    {
        update();
    }

    ContactPoint ContactPointIterator::operator*()
    {
        btPersistentManifold *contactManifold = mData->mDispatcher.getManifoldByIndexInternal(mManifold);

        const btCollisionObject *obj0 = contactManifold->getBody0();
        const btCollisionObject *obj1 = contactManifold->getBody1();

        btManifoldPoint &pt = contactManifold->getContactPoint(mIndex);

        return { pt, obj0, obj1 };
    }

    Proxy<ContactPoint> Engine::Physics::ContactPointIterator::operator->()
    {
        return { **this };
    }

    void ContactPointIterator::operator++()
    {
        assert(mManifold < mData->mDispatcher.getNumManifolds());
        ++mIndex;
        update();
    }

    bool ContactPointIterator::operator!=(const ContactPointIterator &other) const
    {
        assert(mData == other.mData);
        return mManifold != other.mManifold || mIndex != other.mIndex;
    }

    void ContactPointIterator::update()
    {
        while (mManifold < mData->mDispatcher.getNumManifolds() && mIndex == mData->mDispatcher.getManifoldByIndexInternal(mManifold)->getNumContacts()) {
            ++mManifold;
            mIndex = 0;
        }
    }

    void ContactPoint::setFriction(float motion, const Vector3 &dir)
    {
        btManifoldResult result;

        mPoint.m_contactPointFlags |= BT_CONTACT_FLAG_LATERAL_FRICTION_INITIALIZED;

        mPoint.m_lateralFrictionDir1.setValue(dir.x, dir.y, dir.z);
        ///downscale the friction direction 2 for lower (anisotropic) friction (rather than a unit vector)
        mPoint.m_lateralFrictionDir2.setValue(0, 0, 0.1);
        ///choose a target surface velocity in the friction dir1 direction, for a conveyor belt effect
        mPoint.m_contactMotion1 = motion;

        mPoint.m_combinedFriction = /*result.calculateCombinedFriction(mObj0, mObj1)*/0;
        mPoint.m_combinedRestitution = result.calculateCombinedRestitution(mObj0, mObj1);
    }

}
}
