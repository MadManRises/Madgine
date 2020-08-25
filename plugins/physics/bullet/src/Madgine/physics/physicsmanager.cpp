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

    struct PhysicsManager::PhysicsData {
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
        }
    };

    PhysicsManager::PhysicsManager(Scene::SceneManager &sceneMgr)
        : VirtualScope(sceneMgr)
    {
    }

    PhysicsManager::~PhysicsManager()
    {
    }

    btDiscreteDynamicsWorld &Engine::Physics::PhysicsManager::world()
    {
        return mData->mWorld;
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

}
}