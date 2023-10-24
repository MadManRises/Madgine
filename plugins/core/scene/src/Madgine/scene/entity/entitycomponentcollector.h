#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_NAMED_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponent, EntityComponentBase, Engine::UniqueComponent::Constructor<EntityComponentBase, const ObjectPtr &>)
DECLARE_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponentList, EntityComponentListBase, Engine::UniqueComponent::Constructor<EntityComponentListBase>)


namespace Engine {
namespace Scene {
    namespace Entity {

        //MADGINE_SCENE_EXPORT std::map<std::string_view, IndexRef> &sComponentsByName();

    }
}
}