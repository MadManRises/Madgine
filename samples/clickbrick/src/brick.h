#pragma once

#include "Madgine/scene/entity/entitycomponent.h"

#include "Modules/math/vector3.h"
#include "Modules/math/quaternion.h"

namespace ClickBrick {
namespace Scene {

    struct Brick : Engine::Scene::Entity::EntityComponent<Brick> {
        using Engine::Scene::Entity::EntityComponent<Brick>::EntityComponent;

        float mSpeed;
        Engine::Vector3 mDir;

		Engine::Quaternion mQ0, mQ1;
        float mQAcc, mQSpeed;
    };

}
}

RegisterType(ClickBrick::Scene::Brick);