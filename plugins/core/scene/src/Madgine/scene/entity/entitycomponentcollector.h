#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_NAMED_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponent, EntityComponentBase, const ObjectPtr &)
DECLARE_UNIQUE_COMPONENT(Engine::Scene::Entity, EntityComponentList, EntityComponentListBase)


namespace Engine {
namespace Scene {
    namespace Entity {

        //MADGINE_SCENE_EXPORT std::map<std::string_view, IndexRef> &sComponentsByName();

    }
}
}