#pragma once

#include "../entitycomponent.h"

#include "Meta/math/vector3.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT PointLight : EntityComponent<PointLight> {
            PointLight(const ObjectPtr &init);

            uint32_t mRange = 10;
            Vector3 mColor = { 1.0f, 1.0f, 1.0f };
        };

        using PointLightPtr = EntityComponentPtr<PointLight>;

    }
}
}

RegisterType(Engine::Scene::Entity::PointLight);
RegisterType(Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::PointLight>);