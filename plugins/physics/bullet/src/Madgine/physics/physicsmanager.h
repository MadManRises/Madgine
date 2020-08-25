#pragma once

#include "Madgine/scene/scenecomponentbase.h"
#include "Madgine/scene/scenecomponentcollector.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"

namespace Engine {
namespace Physics {

    struct RigidBody;

    struct MADGINE_BULLET_EXPORT PhysicsManager : Scene::SceneComponent<PhysicsManager> {

        PhysicsManager(Scene::SceneManager &sceneMgr);
        ~PhysicsManager();

        btDiscreteDynamicsWorld &world();

    protected:
        virtual bool init() override;
        virtual void finalize() override;

        virtual void update(std::chrono::microseconds) override;

    private:
        struct PhysicsData;
        std::unique_ptr<PhysicsData> mData;
    };

}
}

RegisterType(Engine::Physics::PhysicsManager);